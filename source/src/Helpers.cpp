#include "Global.h"
#include "Helpers.h"

const char* CharVectorToString(std::vector<char>* vect)
{
    if (!vect)
        return NULL;

    char* ret = new char[vect->size()];
    uint32 i = 0;
    for (std::vector<char>::const_iterator itr = vect->begin(); itr != vect->end(); ++itr)
        ret[i++] = (*itr);

    return ret;
}

char* ExtractFolderFromPath(const char* input)
{
    for (int32 i = strlen(input); i >= 0; i--)
        if (input[i] == '\\')
        {
            char* tmp = new char[i+1];
            strncpy(tmp, input, i);
            tmp[i] = '\0';
            return tmp;
        }

    return NULL;
}

char* ExtractFilenameFromPath(const char* input)
{
    for (int32 i = strlen(input); i > 0; i--)
    {
        if (input[i-1] == '\\')
        {
            char* tmp = new char[strlen(input)-i+1];
            strncpy(tmp, (input+i), strlen(input)-i);
            tmp[strlen(input)-i] = '\0';
            return tmp;
        }
    }

    return NULL;
}

char* MakeFilePath(const char* dir, const char* filename)
{
    if (!dir)
        return (char*)filename;

    if (!filename)
        return NULL;

    char* tmp = NULL;

    if (dir[strlen(dir)-1] != '\\')
    {
        tmp = new char[strlen(dir)+1+strlen(filename)+1];
        sprintf(tmp, "%s\\%s", dir, filename);
    }
    else
    {
        tmp = new char[strlen(dir)+strlen(filename)+1];
        sprintf(tmp, "%s%s", dir, filename);
    }

    return tmp;
}

char* LeftSide(const char* input, const char delim)
{
    if (!input || strlen(input) < 1)
        return NULL;

    for (uint32 i = 0; i < strlen(input); i++)
    {
        if (input[i] == delim)
        {
            char* tmp = new char[i+1];
            strncpy(tmp, input, i);
            tmp[i] = '\0';
            return tmp;
        }
    }

    return (char*)input;
}

char* RightSide(const char* input, const char delim)
{
    if (!input || strlen(input) < 1)
        return NULL;

    for (uint32 i = 0; i < strlen(input); i++)
    {
        if (input[i] == delim)
        {
            char* tmp = new char[strlen(input)-i+1];
            strncpy(tmp, (input+i+1), strlen(input)-i-1);
            tmp[strlen(input)-i-1] = '\0';
            return tmp;
        }
    }

    return NULL;
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

bool IsNumeric(const char* inp)
{
    if (!inp || strlen(inp) < 1)
        return false;

    for (uint32 i = 0; i < strlen(inp); i++)
        if (inp[i] < '0' || inp[i] > '9')
            return false;

    return true;
}

int ToInt(const char* inp)
{
    return strtol(inp, (char**)&inp, 10);
}

char* RemoveBeginningSpaces(const char* input)
{
    if (!input)
        return NULL;

    if (input[0] != ' ')
        return (char*)input;

    for (uint32 i = 0; i < strlen(input); i++)
    {
        if (input[i] != ' ')
        {
            char* tmp = new char[strlen(input)-i];
            strncpy(tmp, (input+i), strlen(input)-i);
            tmp[strlen(input)-i] = '\0';
            return tmp;
        }
    }

    return NULL;
}
