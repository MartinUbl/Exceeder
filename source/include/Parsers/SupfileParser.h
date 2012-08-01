#ifndef EXCDR_SUPFILE_PARSER_H
#define EXCDR_SUPFILE_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"

static const char* SupportedSupfileVersions[] = {
    "1.0",
    "test"
};

class SupfileParser: public Parser
{
    public:
        static bool Parse(std::vector<std::string>* input);
};

#endif
