#include "Global.h"
#include "Application.h"
#include "Parsers/Parser.h"
#include "Log.h"
#include "Storage.h"

wchar_t* Parser::ReadLine(FILE* f)
{
    if (!f)
        return NULL;

    std::vector<wchar_t> charbuf;
    while (int32 chr = getwc(f))
    {
        if (charbuf.empty() && chr == WEOF)
            return NULL;

        if (chr == '\n' || chr == WEOF)
        {
            charbuf.push_back('\0');
            return (wchar_t*)CharVectorToString(&charbuf);
        }

        charbuf.push_back(chr);
    }

    return NULL;
}

bool Parser::PrepareLine(wchar_t *&input)
{
    // valid line has 1 and more characters
    if (wcslen(input) < 1)
        return false;

    // indicate comments and cut the string
    bool comment = false;
    for (uint32 i = 0; i < wcslen(input)-1; i++)
    {
        if (input[i] == '/' && input[i+1] == '/')
            comment = true;

        if (comment)
        {
            input[i] = '\0';
            break;
        }
    }

    // verify that line consists of valid characters - not only from spaces
    bool validline = false;
    for (uint32 i = 0; i < wcslen(input); i++)
    {
        if (input[i] != ' ')
        {
            validline = true;
            break;
        }
    }

    if (!validline)
        return false;

    // cut the whitespaces off
    for (int32 i = wcslen(input)-1; i >= 0; i--)
    {
        if (input[i] == ' ')
            input[i] = '\0';
        else
            break;
    }

    input = RemoveBeginningSpaces(input);

    return true;
}

bool Parser::PreParseLine(wchar_t *&line)
{
    // true  = line has to be parsed by main parser
    // false = line was parsed by this parser - don't parse anymore

    // Parse macros
    wchar_t* left = LeftSide(line, ' ');
    wchar_t* right = RightSide(line, ' ');
    int pos = -1;

    if (!left)
        return true;

    while ((pos = ContainsString(line, L"{#")) != -1 && wcslen(line) > pos+2)
    {
        wchar_t* tmp = LeftSide(&line[pos+2], '}');
        wchar_t* tmpright = RightSide(&line[pos+2], '}');
        if (tmp)
        {
            const wchar_t* val = sStorage->GetMacroValue(tmp);

            uint32 newsize = pos-1  +  wcslen(val)  +  wcslen(tmpright)  +1;
            tmp = new wchar_t[newsize];
            memset(tmp, 0, sizeof(wchar_t)*newsize);

            wcsncpy(tmp, line, pos);

            swprintf(tmp, newsize+1, L"%s%s%s", tmp, val, tmpright);

            line = tmp;
        }
    }

    if (EqualString(left, L"\\MACRO"))
    {
        if (!right)
            return true;

        wchar_t* ident = LeftSide(right, ' ');
        right = RightSide(right, ' ');

        if (!ident || !right)
        {
            sLog->ErrorLog("Parser: invalid macro definition '%S'", line);
            return true;
        }

        if (wcslen(ident) < 2)
            sLog->ErrorLog("Parser: Warning: macro defined with line '%S' has too short identificator", line);

        if (!sStorage->AddMacro(ident, right))
        {
            sLog->ErrorLog("Parsed: Macro with identificator '%S' is already defined! Ignored.", ident);
            return true;
        }

        return false;
    }

    return true;
}
