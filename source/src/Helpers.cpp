#include "Global.h"
#include "Helpers.h"

#include <sstream>

wchar_t* CharVectorToString(std::vector<wchar_t>* vect)
{
    if (!vect)
        return NULL;

    wchar_t* ret = new wchar_t[vect->size()+1];
    uint32 i;
    std::wstring a;
    for (i = 0; i < vect->size(); i++)
        a += (*vect)[i];

    wcsncpy(ret, a.c_str(), a.size());
    ret[vect->size()] = L'\0';

    return ret;
}

wchar_t* ExtractFolderFromPath(const wchar_t* input)
{
    for (int32 i = wcslen(input); i >= 0; i--)
        if (input[i] == '\\')
        {
            wchar_t* tmp = new wchar_t[i+1];
            wcsncpy(tmp, input, i);
            tmp[i] = '\0';
            return tmp;
        }

    return NULL;
}

wchar_t* ExtractFilenameFromPath(const wchar_t* input)
{
    for (int32 i = wcslen(input); i > 0; i--)
    {
        if (input[i-1] == '\\')
        {
            wchar_t* tmp = new wchar_t[wcslen(input)-i+1];
            wcsncpy(tmp, (input+i), wcslen(input)-i);
            tmp[wcslen(input)-i] = '\0';
            return tmp;
        }
    }

    return NULL;
}

wchar_t* MakeFilePath(const wchar_t* dir, const wchar_t* filename)
{
    if (!dir)
        return (wchar_t*)filename;

    if (!filename)
        return NULL;

    wchar_t* tmp = NULL;

    if (dir[wcslen(dir)-1] != '\\')
    {
        tmp = new wchar_t[wcslen(dir)+1+wcslen(filename)+1];
        swprintf(tmp, 99999, L"%s\\%s", dir, filename);
    }
    else
    {
        tmp = new wchar_t[wcslen(dir)+wcslen(filename)+1];
        swprintf(tmp, 99999, L"%s%s", dir, filename);
    }

    return tmp;
}

wchar_t* LeftSide(const wchar_t* input, const wchar_t delim)
{
    if (!input || wcslen(input) < 1)
        return NULL;

    for (uint32 i = 0; i < wcslen(input); i++)
    {
        if (input[i] == delim)
        {
            wchar_t* tmp = new wchar_t[i+1];
            wcsncpy(tmp, input, i);
            tmp[i] = '\0';
            return tmp;
        }
    }

    return (wchar_t*)input;
}

wchar_t* RightSide(const wchar_t* input, const wchar_t delim)
{
    if (!input || wcslen(input) < 1)
        return NULL;

    for (uint32 i = 0; i < wcslen(input); i++)
    {
        if (input[i] == delim)
        {
            wchar_t* tmp = new wchar_t[wcslen(input)-i+1];
            wcsncpy(tmp, (input+i+1), wcslen(input)-i-1);
            tmp[wcslen(input)-i-1] = '\0';
            return tmp;
        }
    }

    return NULL;
}

bool EqualString(const wchar_t* first, const wchar_t* second, bool caseInsensitive)
{
    if (wcslen(first) != wcslen(second))
        return false;

    if (caseInsensitive)
    {
        for (uint32 i = 0; i < wcslen(first); i++)
            if (UpperChar(first[i]) != UpperChar(second[i]))
                return false;
    }
    else
    {
        for (uint32 i = 0; i < wcslen(first); i++)
            if (first[i] != second[i])
                return false;
    }

    return true;
}

bool EqualString(const char* first, const char* second)
{
    if (strlen(first) != strlen(second))
        return false;

    for (uint32 i = 0; i < strlen(first); i++)
        if (first[i] != second[i])
            return false;

    return true;
}

int ContainsString(const wchar_t* str, const wchar_t* substr)
{
    if (!str || !substr)
        return -1;

    if (wcslen(substr) > wcslen(str))
        return -1;

    bool wrong;

    for (uint32 i = 0; i < wcslen(str)-wcslen(substr); i++)
    {
        if (str[i] == substr[0])
        {
            wrong = false;
            for (uint32 j = 0; j < wcslen(substr); j++)
            {
                if (str[i+j] != substr[j])
                {
                    wrong = true;
                    break;
                }
            }

            if (!wrong)
                return i;
        }
    }

    return -1;
}

bool IsNumeric(const wchar_t* inp)
{
    if (!inp || wcslen(inp) < 1)
        return false;

    for (uint32 i = 0; i < wcslen(inp); i++)
        if ((i == 0 && inp[i] != '-') && (inp[i] < '0' || inp[i] > '9'))
            return false;

    return true;
}

int ToInt(const wchar_t* inp)
{
    return wcstol(inp, (wchar_t**)&inp, 10);
}

wchar_t* RemoveBeginningSpaces(const wchar_t* input)
{
    if (!input)
        return NULL;

    if (input[0] != ' ')
        return (wchar_t*)input;

    for (uint32 i = 0; i < wcslen(input); i++)
    {
        if (input[i] != ' ')
        {
            wchar_t* tmp = new wchar_t[wcslen(input)-i];
            wcsncpy(tmp, (input+i), wcslen(input)-i);
            tmp[wcslen(input)-i] = '\0';
            return tmp;
        }
    }

    return NULL;
}

void ParseInputDefinitions(wchar_t* input, ParsedDefs* output)
{
    wchar_t* tmp = NULL;
    while (input != NULL && wcslen(input) > 0)
    {
        tmp = LeftSide(input, '}');

        // no definitions?
        if (wcslen(tmp) == wcslen(input))
            break;

        output->push_back(std::make_pair(LeftSide(tmp, ':'), RightSide(tmp, ':')));

        input = RightSide(input, '{');
    }
}

const wchar_t* GetDefinitionKeyValue(ParsedDefs* input, const wchar_t* key)
{
    if (!input || !key || wcslen(key) < 1)
        return L"";

    const wchar_t* upkey = ToUppercase(key);

    for (ParsedDefs::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        if (EqualString(ToUppercase((*itr).first), upkey, true))
            return (*itr).second.c_str();
    }

    return L"";
}

void GetPositionDefinitionKeyValue(ParsedDefs* input, const wchar_t* key, int32* destX, int32* destY)
{
    (*destX) = 0;
    (*destY) = 0;

    if (const wchar_t* pos = GetDefinitionKeyValue(input, key))
    {
        if (wcslen(pos) > 0)
        {
            const wchar_t* xpos = LeftSide(pos, ',');
            const wchar_t* ypos = RightSide(pos, ',');
            if (IsNumeric(xpos) && IsNumeric(ypos))
            {
                if (destX)
                    (*destX) = ToInt(xpos);
                if (destY)
                    (*destY) = ToInt(ypos);
            }
        }
    }
}

static const wchar_t* patt_lowcase = {L"abcdefghijklmnopqrstuvwxyz"};
static const wchar_t* patt_upcase  = {L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

wchar_t UpperChar(wchar_t inp)
{
    for (uint32 i = 0; i < wcslen(patt_lowcase); i++)
        if (patt_lowcase[i] == inp)
            return patt_upcase[i];

    return inp;
}

wchar_t LowerChar(wchar_t inp)
{
    for (uint32 i = 0; i < wcslen(patt_upcase); i++)
        if (patt_upcase[i] == inp)
            return patt_lowcase[i];

    return inp;
}

const wchar_t* ToUppercase(const wchar_t* input)
{
    wchar_t* ret = new wchar_t[wcslen(input)+1];

    for (uint32 i = 0; i < wcslen(input); i++)
        ret[i] = UpperChar(input[i]);

    ret[wcslen(input)] = '\0';

    return ret;
}

const wchar_t* ToLowercase(const wchar_t* input)
{
    std::wstringstream ss;

    for (uint32 i = 0; i < wcslen(input); i++)
        ss << LowerChar(input[i]);

    ss << wchar_t(0);

    wchar_t* pp = new wchar_t[ss.str().size()];
    wcsncpy(pp, ss.str().c_str(), ss.str().size());

    return pp;
}

const wchar_t* ToWideString(const char* input)
{
    const size_t origsize = strlen(input)+1;
    wchar_t* wc = new wchar_t[origsize];
    mbstowcs(wc, input, origsize);

    return wc;
}

const char* ToMultiByteString(const wchar_t* input)
{
    const size_t origsize = wcslen(input)+1;
    char* mbc = new char[origsize];
    wcstombs(mbc, input, origsize);

    return mbc;
}
