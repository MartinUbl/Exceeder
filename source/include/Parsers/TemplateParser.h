#ifndef EXCDR_TEMPLATE_PARSER_H
#define EXCDR_TEMPLATE_PARSER_H

#include "Global.h"
#include "Parsers/Parser.h"
#include "Defines/Templates.h"

static const wchar_t* SupportedTemplateVersions[] = {
    L"1.0",
    L"test"
};

class TemplateParser: public Parser
{
    public:
        static bool ParseFile(const wchar_t* path);
        static bool Parse(std::vector<std::wstring>* input);
};

#endif
