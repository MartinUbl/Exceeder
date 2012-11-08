#ifndef EXCDR_SUPFILE_PARSER_H
#define EXCDR_SUPFILE_PARSER_H

#include "Global.h"
#include "Parsers/Parser.h"

static const wchar_t* SupportedSupfileVersions[] = {
    L"1.0",
    L"test"
};

class SupfileParser: public Parser
{
    public:
        static bool Parse(std::vector<std::wstring>* input);
};

#endif
