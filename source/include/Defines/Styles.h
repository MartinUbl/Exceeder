#ifndef EXCDR_STYLES_H
#define EXCDR_STYLES_H

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

enum TextPosition
{
    TEXT_POSITION_LEFT   = 0,
    TEXT_POSITION_CENTER = 1,
    TEXT_POSITION_RIGHT  = 2
};

struct Style
{
    Style()
    {
        memset(this, 0, sizeof(Style));
    }

    const char* fontFamily;
    uint32*     fontSize;
    uint32*     fontColor;

    int32       fontId;                  // built font id - created after style definition (DEF_END)

    TextPosition* textPosition;
};

typedef std::map<const char*, Style*> StyleMap;

#endif
