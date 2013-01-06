#ifndef EXCDR_PRESENTATION_H
#define EXCDR_PRESENTATION_H

#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/StyleParser.h"
#include "Defines/Slides.h"
#include "Defines/Styles.h"
#include "Defines/Effects.h"
#include "Handlers/EffectHandler.h"
#include "Vector.h"

enum InterfaceEventTypes
{
    IE_MOUSE_LEFT_DOWN      = 0,
    IE_MOUSE_LEFT_UP        = 1,
    IE_MOUSE_RIGHT_DOWN     = 2,
    IE_MOUSE_RIGHT_UP       = 3,
    IE_MOUSE_MOVE           = 4,
    IE_KEYBOARD_PRESS       = 5,
    IE_KEYBOARD_RELEASE     = 6,
    IE_EFFECT_END           = 7,
    IE_MAX
};

// We have to define this since on Linux we handle the cycle with internal GLUT loop unlike on Windows - on Windows, we have our own cycle
#ifdef _WIN32
#define PRESENTATION_CONTINUE continue
#define PRESENTATION_BREAK break
#else
#define PRESENTATION_CONTINUE return
#define PRESENTATION_BREAK return
#endif

typedef std::list<SlideElement*> SlideList;

class PresentationMgr
{
    public:
        PresentationMgr();

        bool Init();
        void Run();

        void InterfaceEvent(InterfaceEventTypes type, int32 param1 = 0, int32 param2 = 0);
        void HandleBluetoothMessage(char* msg, uint8 len);

        void SetBlocking(bool block) { m_blocking = block; };
        bool IsBlocking() { return m_blocking; };

        SlideElement* GetActiveElementById(const wchar_t* id);

        int64 NumerateExpression(ExpressionTreeElement* expr);
        int64 GetElementReferenceValue(wchar_t* input);

    private:
        uint32 m_slideElementPos;
        SlideElement* m_slideElement;
        SlideList m_activeElements;

        struct BackgroundData
        {
            uint32 color;
            uint32 resourceId;
            uint32 backgroundPosition[2];
            uint32 backgroundDimensions[2];
            SlideElement* source;         // in case of recalculating sizes (changing of screen resolution 'on demand', and so)
        } bgData;

        // These things are used for "hard" canvas effects
        // this means, that every element on the canvas will be affected, regardless of order
        struct CanvasLayer
        {
            // canvas offset
            CVector2 hardMove;
            EffectTime hardMove_time;

            // canvas rotation
            int32 hardRotateCenter[2];
            float hardRotateAngle;
            EffectTime hardRotate_time;

            // canvas scale
            float hardScale;
            EffectTime hardScale_time;

            // canvas blur
            float hardBlur;
            EffectTime hardBlur_time;
        } canvas;

        bool m_blocking;

        bool m_btEnabled;
#ifdef _WIN32
        HANDLE m_btHandle;
#endif
};

#define sPresentation Singleton<PresentationMgr>::instance()

#endif
