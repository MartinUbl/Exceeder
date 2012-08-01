#ifndef EXCDR_STYLE_PARSER_H
#define EXCDR_STYLE_PARSER_H

#include "Global.h"
#include "Parsers\Parser.h"

static const char* SupportedStyleVersions[] = {
    "1.0",
    "test"
};

struct KnownColor
{
    const char* name;
    uint32 color;
};

static const KnownColor KnownColors[] = {
    {"RED",     0xFF000000},
    {"GREEN",   0x00FF0000},
    {"BLUE",    0x0000FF00},
    {"WHITE",   0xFFFFFF00},
    {"BLACK",   0x00000000}
};

#define MAKE_COLOR(r,g,b,a) ((uint32(r) << 24) | (uint32(uint8(g)) << 16) | (uint32(uint8(b)) << 8) | (uint8(a)))
#define COLOR_R(c) uint8(c >> 24)
#define COLOR_G(c) uint8(c >> 16)
#define COLOR_B(c) uint8(c >> 8)
#define COLOR_A(c) uint8(c)

class StyleParser: public Parser
{
    public:
        static bool ParseFile(const char* path);
        static bool Parse(std::vector<std::string>* input);
        static bool ParseColor(const char* input, uint32 *dest);
};

#endif
