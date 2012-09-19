#ifndef EXCDR_EFFECT_PARSER_H
#define EXCDR_EFFECT_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"
#include "Parsers\StyleParser.h"

static const char* SupportedEffectsVersions[] = {
    "1.0",
    "test"
};

class EffectParser: public Parser
{
    public:
        static bool ParseFile(const char* path);
        static bool Parse(std::vector<std::string>* input);
};

#endif
