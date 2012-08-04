#include "Presentation.h"
#include <ctime>

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return sSimplyFlat->SFWndProc(hWnd, msg, wParam, lParam);
}

PresentationMgr::PresentationMgr()
{
    m_slideElementPos = 0;
    m_slideElement = NULL;
    m_blocking = false;

    memset(&bgData, 0, sizeof(BackgroundData));
}

/////
void KeyPressed(uint16 key, bool pressed)
{
    if (pressed)
        sPresentation->InterfaceEvent(IE_KEYBOARD_PRESS, key);
    else
        sPresentation->InterfaceEvent(IE_KEYBOARD_RELEASE, key);
}
/////

bool PresentationMgr::Init()
{
    if (!sSimplyFlat->CreateMainWindow("Exceeder Presentation", sStorage->GetScreenWidth(), sStorage->GetScreenHeight(),
        32, false, 60, &MyWndProc))
        RAISE_ERROR("Could not initialize main window!");

    sSimplyFlat->Interface->HookEvent(VK_RETURN, KeyPressed);

    srand((unsigned int)(time(NULL)));

    return true;
}

void PresentationMgr::InterfaceEvent(InterfaceEventTypes type, int32 param1, int32 param2)
{
    switch (type)
    {
        case IE_KEYBOARD_PRESS:
            if (m_slideElement && m_slideElement->elemType == SLIDE_ELEM_KEYBOARD_EVENT &&
                (m_slideElement->typeKeyboardEvent.key == 0 || m_slideElement->typeKeyboardEvent.key == param1) &&
                m_slideElement->typeKeyboardEvent.type == KEYBOARD_EVENT_KEY_DOWN)
                SetBlocking(false);
            break;
        case IE_KEYBOARD_RELEASE:
            if (m_slideElement && m_slideElement->elemType == SLIDE_ELEM_KEYBOARD_EVENT &&
                (m_slideElement->typeKeyboardEvent.key == 0 || m_slideElement->typeKeyboardEvent.key == param1) &&
                m_slideElement->typeKeyboardEvent.type == KEYBOARD_EVENT_KEY_UP)
                SetBlocking(false);
            break;
        case IE_MOUSE_LEFT_DOWN:
            // Verifies if blocking event is mouse event for left button down,
            // if click position is equal to 0 (line) or if clicked point is within the click position
            if (m_slideElement && m_slideElement->elemType == SLIDE_ELEM_MOUSE_EVENT &&
                m_slideElement->typeMouseEvent.type == MOUSE_EVENT_LEFT_DOWN &&
                ((m_slideElement->typeMouseEvent.positionSquareLU[0] == 0 && m_slideElement->typeMouseEvent.positionSquareRL[0] == 0) ||
                 IN_SQUARE(param1, param2, m_slideElement->typeMouseEvent.positionSquareLU[0],
                    m_slideElement->typeMouseEvent.positionSquareLU[1],
                    m_slideElement->typeMouseEvent.positionSquareRL[0],
                    m_slideElement->typeMouseEvent.positionSquareRL[1])))
                SetBlocking(false);
            break;
        default:
            break;
    }
}

bool PresentationMgr::Run()
{
    MSG msg;
    SlideElement* tmp;

    while (true)
    {
        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                break;

            continue;
        }

        sSimplyFlat->BeforeDraw();

        if (bgData.resourceId > 0)
        {
            // TODO
        }
        else
        {
            sSimplyFlat->Drawing->ClearColor(COLOR_R(bgData.color),COLOR_G(bgData.color),COLOR_B(bgData.color));
        }

        // draw active elements which should be drawn
        for (SlideList::iterator itr = m_activeElements.begin(); itr != m_activeElements.end(); ++itr)
            (*itr)->Draw();

        sSimplyFlat->AfterDraw();

        if (m_blocking)
            continue;

        tmp = sStorage->GetSlideElement(m_slideElementPos);
        if (!tmp)
            break;

        m_slideElement = new SlideElement;
        memcpy(m_slideElement, tmp, sizeof(SlideElement));
        tmp = NULL;

        m_activeElements.push_back(m_slideElement);

        switch (m_slideElement->elemType)
        {
            case SLIDE_ELEM_MOUSE_EVENT:
                m_blocking = true;
                break;
            case SLIDE_ELEM_BACKGROUND:
                if (m_slideElement->typeBackground.imageResourceId > 0)
                    bgData.resourceId = m_slideElement->typeBackground.imageResourceId;

                bgData.color = m_slideElement->typeBackground.color;
                break;
            default:
                break;
        }

        m_slideElementPos++;
    }

    return true;
}
