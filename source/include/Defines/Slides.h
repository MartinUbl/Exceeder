#ifndef EXCDR_SLIDES_H
#define EXCDR_SLIDES_H

#include "Parsers/ExpressionParser.h"

typedef std::map<uint32, ExpressionTreeElement> ExprMap;

enum SlideElementTypes
{
    SLIDE_ELEM_NONE             = 0,
    SLIDE_ELEM_TEXT             = 1,
    SLIDE_ELEM_BACKGROUND       = 2,
    SLIDE_ELEM_MOUSE_EVENT      = 3,
    SLIDE_ELEM_KEYBOARD_EVENT   = 4,
    SLIDE_ELEM_NEW_SLIDE        = 5,
    SLIDE_ELEM_IMAGE            = 6,
    SLIDE_ELEM_PLAY_EFFECT      = 7,
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

enum KeyboardEventTypes
{
    KEYBOARD_EVENT_NONE         = 0,
    KEYBOARD_EVENT_KEY_DOWN     = 1,
    KEYBOARD_EVENT_KEY_UP       = 2,
    KEYBOARD_EVENT_MAX
};

#define SPEC_BASE -INT_MAX

enum PositionSpecial
{
    POS_CENTER     = SPEC_BASE+1,
    POS_LEFT       = SPEC_BASE+2,
    POS_RIGHT      = SPEC_BASE+3,
    POS_TOP        = SPEC_BASE+4,
    POS_BOTTOM     = SPEC_BASE+5
};

enum Spread
{
    SPREAD_NONE   = 0,
    SPREAD_WIDTH  = 1,
    SPREAD_HEIGHT = 2,
    SPREAD_BOTH   = 3
};

struct GradientData
{
    uint32 color;
    uint32 size;
};

enum GradientArray
{
    GRAD_TOP    = 0,
    GRAD_RIGHT  = 1,
    GRAD_LEFT   = 2,
    GRAD_BOTTOM = 3,
    GRAD_MAX
};

struct KnownKey
{
    const wchar_t* name;
    uint16 code;
};

static const KnownKey KnownKeys[] = {
    {L"ENTER",     VK_RETURN},
    {L"RETURN",    VK_RETURN},
    {L"SPACE",     VK_SPACE},
    {L"CTRL",      VK_CONTROL},
    {L"SHIFT",     VK_SHIFT}
};

class EffectHandler;

struct SlideElement
{
    SlideElement()
    {
        elemId = L"";
        elemStyle = L"";
        elemEffect = L"";

        myEffect = NULL;
        drawable = false;

        for (uint32 i = 0; i <= 1; i++)
        {
            position[i] = 0;
            finalPosition[i] = 0;
        }

        typeText.outlist = NULL;
    }

    SlideElementTypes elemType;
    bool drawable;

    std::wstring elemId;
    std::wstring elemStyle;
    std::wstring elemEffect;

    EffectHandler* myEffect;

    int32 position[2]; // element position
    int32 finalPosition[2]; // final position in case of effects - used for calculating wrapping limits

    void CreateEffectIfAny();
    void PlayEffect(const wchar_t* effectId);
    void Draw();

    // Drawable slide element data

    struct elemTextData
    {
        std::wstring text;       // text... text!
        StyledTextList* outlist; // prepared render list for case of marked up input
        ExprMap outlistExpressions; // map of expressions prepared
        uint32 depth;            // depth of drawing - for some kind of "layers"
        int32 wrapSign;          // sign for wrapping, default prewrapped
        void Draw(SlideElement* parent);
        static uint8 GetFeatureArrayIndexOf(Style* style);
    } typeText;

    // Event-based / static content-based / other nondrawable element data

    struct elemBackgroundData
    {
        uint32 color;               // in case of background color change
        uint32 imageResourceId;     // internal ID of loaded resource
        int32 position[2];          // horizontal and vertical position (numbers or enum value from PositionSpecial
        int32 dimensions[2];        // width and height
        Spread spread;              // background axis spread
        GradientData* gradients[GRAD_MAX]; // gradient data - each side has its own pointer - the pointers can point to the same piece of memory to share settings
    } typeBackground;

    struct elemMouseEventData
    {
        MouseEventTypes type;       // type of mouse event
        uint32 positionSquareLU[2]; // left upper corner of position
        uint32 positionSquareRL[2]; // right lower corner of position
    } typeMouseEvent;

    struct elemKeyboardEventData
    {
        KeyboardEventTypes type;   // type of kb event
        uint16 key;                // virtual key id
    } typeKeyboardEvent;

    struct elemImageData
    {
        uint32 resourceId;         // internal id of resource
        uint32 size[2];            // size of rectangle - width, height
        void Draw(SlideElement* parent);
    } typeImage;
};

typedef std::vector<SlideElement*> SlideElementVector;

#endif
