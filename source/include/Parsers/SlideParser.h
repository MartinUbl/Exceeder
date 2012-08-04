#ifndef EXCDR_SLIDE_PARSER_H
#define EXCDR_SLIDE_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"

static const char* SupportedSlideVersions[] = {
    "1.0",
    "test"
};

class SlideParser: public Parser
{
    public:
        static bool ParseFile(const char* path);
        static bool Parse(std::vector<std::string>* input);
};

#endif
