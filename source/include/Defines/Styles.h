#ifndef EXCDR_STYLES_H
#define EXCDR_STYLES_H

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

    TextPosition* textPosition;
};

typedef std::map<const char*, Style*> StyleMap;

#endif
