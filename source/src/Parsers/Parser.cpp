#include "Global.h"
#include "Application.h"
#include "Parsers/Parser.h"

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
            input[i] = '\0';
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
