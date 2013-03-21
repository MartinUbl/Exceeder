#ifndef EXCDR_SLIDE_PARSER_H
#define EXCDR_SLIDE_PARSER_H

#include "Global.h"
#include "Parsers/Parser.h"

static const wchar_t* SupportedSlideVersions[] = {
    L"1.0",
    L"test"
};

enum SlideElementParseFlag
{
    SEPF_NONE          = 0x00,
    SEPF_NON_TEMPLATE  = 0x01,
    SEPF_ONLY_TEMPLATE = 0x02,
    SEPF_REPORT_ERROR  = 0x04,
};

class SlideParser: public Parser
{
    public:
        static bool ParseFile(const wchar_t* path);
        static bool Parse(std::vector<std::wstring>* input);
        static void ParseMarkup(const wchar_t* input, const wchar_t* stylename, StyledTextList* target, ExprMap* exmap);
        static uint16 ResolveKey(const wchar_t* input);
        static SlideElement* ParseElement(const wchar_t* input, uint8* special = NULL, wchar_t** persistentIdentificator = NULL);
};

#endif
