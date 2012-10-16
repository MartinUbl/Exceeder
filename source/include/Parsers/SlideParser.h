#ifndef EXCDR_SLIDE_PARSER_H
#define EXCDR_SLIDE_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"

static const wchar_t* SupportedSlideVersions[] = {
    L"1.0",
    L"test"
};

class SlideParser: public Parser
{
    public:
        static bool ParseFile(const wchar_t* path);
        static bool Parse(std::vector<std::wstring>* input);
        static uint16 ResolveKey(const wchar_t* input);
};

#endif
