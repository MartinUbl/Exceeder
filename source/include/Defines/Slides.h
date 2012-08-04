#ifndef EXCDR_SLIDES_H
#define EXCDR_SLIDES_H

enum SlideElementTypes
{
    SLIDE_ELEM_NONE             = 0,
    SLIDE_ELEM_TEXT             = 1,
    SLIDE_ELEM_BACKGROUND       = 2,
    SLIDE_ELEM_MOUSE_EVENT      = 3,
    SLIDE_ELEM_MAX
};

enum MouseEventTypes
{
    MOUSE_EVENT_NONE            = 0,
    MOUSE_EVENT_LEFT_DOWN       = 1,
    MOUSE_EVENT_LEFT_UP         = 2,
    MOUSE_EVENT_RIGHT_DOWN      = 3,
    MOUSE_EVENT_RIGHT_UP        = 4,
    MOUSE_EVENT_MOVE            = 5,
    MOUSE_EVENT_MAX
};

struct SlideElement
{
    SlideElement()
    {
        elemId = "";
        elemStyle = "";
        elemEffect = "";
    }

    SlideElementTypes elemType;

    std::string elemId;
    std::string elemStyle;
    std::string elemEffect;

    struct elemTextData
    {
        uint32 position[2]; // text position
        std::string text;   // text... text!
        uint32 depth;       // depth of drawing - for some kind of "layers"
    } typeText;

    struct elemBackgroundData
    {
        uint32 color;           // in case of background color change
        uint32 imageResourceId; // NYI
    } typeBackground;

    struct elemMouseEventData
    {
        MouseEventTypes type;       // type of mouse event
        uint32 positionSquareLU[2]; // left upper corner of position
        uint32 positionSquareRL[2]; // right lower corner of position
    } typeMouseEvent;
};

typedef std::vector<SlideElement*> SlideElementVector;

#endif
