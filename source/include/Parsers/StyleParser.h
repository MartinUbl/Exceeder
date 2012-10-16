#ifndef EXCDR_STYLE_PARSER_H
#define EXCDR_STYLE_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"
#include "Defines\Styles.h"

static const wchar_t* SupportedStyleVersions[] = {
    L"1.0",
    L"test"
};

class StyleParser: public Parser
{
    public:
        static bool ParseFile(const wchar_t* path);
        static bool Parse(std::vector<std::wstring>* input);
        static bool ParseColor(const wchar_t* input, uint32 *dest);
};

#endif
