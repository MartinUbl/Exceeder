#ifndef EXCDR_STYLE_PARSER_H
#define EXCDR_STYLE_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"
#include "Defines\Styles.h"

static const char* SupportedStyleVersions[] = {
    "1.0",
    "test"
};

class StyleParser: public Parser
{
    public:
        static bool ParseFile(const char* path);
        static bool Parse(std::vector<std::string>* input);
        static bool ParseColor(const char* input, uint32 *dest);
};

#endif
