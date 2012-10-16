#ifndef EXCDR_EFFECT_PARSER_H
#define EXCDR_EFFECT_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"
#include "Parsers\StyleParser.h"

static const wchar_t* SupportedEffectsVersions[] = {
    L"1.0",
    L"test"
};

class EffectParser: public Parser
{
    public:
        static bool ParseFile(const wchar_t* path);
        static bool Parse(std::vector<std::wstring>* input);
};

#endif
