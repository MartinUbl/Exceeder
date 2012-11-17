#ifndef EXCDR_RESOURCE_PARSER_H
#define EXCDR_RESOURCE_PARSER_H

#include "Global.h"
#include "Parsers/Parser.h"
#include "Resources.h"

static const wchar_t* SupportedResourceVersions[] = {
    L"1.0",
    L"test"
};

class ResourceParser: public Parser
{
    public:
        static bool ParseFile(const wchar_t* path);
        static bool Parse(std::vector<std::wstring>* input);
};

#endif
