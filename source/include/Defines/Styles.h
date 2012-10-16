#ifndef EXCDR_STYLES_H
#define EXCDR_STYLES_H

struct KnownColor
{
    const wchar_t* name;
    uint32 color;
};

static const KnownColor KnownColors[] = {
    {L"RED",     0xFF000000},
    {L"GREEN",   0x00FF0000},
    {L"BLUE",    0x0000FF00},
    {L"WHITE",   0xFFFFFF00},
    {L"BLACK",   0x00000000}
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

    const wchar_t* fontFamily;
    uint32*     fontSize;
    uint32*     fontColor;
    bool        bold;
    bool        italic;
    bool        underline;
    bool        strikeout;

    int32       fontId;                  // built font id - created after style definition (DEF_END)

    TextPosition* textPosition;
};

typedef std::map<const wchar_t*, Style*> StyleMap;

#endif
