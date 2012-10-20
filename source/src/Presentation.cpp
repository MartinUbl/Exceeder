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

void MouseButtonPress(bool left, bool pressed)
{
    if (left)
    {
        if (pressed)
            sPresentation->InterfaceEvent(IE_MOUSE_LEFT_DOWN);
        else
            sPresentation->InterfaceEvent(IE_MOUSE_LEFT_UP);
    }
    else
    {
        if (pressed)
            sPresentation->InterfaceEvent(IE_MOUSE_RIGHT_DOWN);
        else
            sPresentation->InterfaceEvent(IE_MOUSE_RIGHT_UP);
    }
}
/////

bool PresentationMgr::Init()
{
    // Use SimplyFlat framework to initialize everything for us
    if (!sSimplyFlat->CreateMainWindow("Exceeder Presentation", sStorage->GetScreenWidth(), sStorage->GetScreenHeight(),
        32, false, 60, &MyWndProc))
        RAISE_ERROR("Could not initialize main window!");

    // Hook events usable for our presentation mode (key press/release + mouse button press)
    sSimplyFlat->Interface->HookEvent(0, KeyPressed);
    sSimplyFlat->Interface->HookMouseEvent(MouseButtonPress);

    // Default font will be Arial, normal, size 25px
    sStorage->SetDefaultFontId(sSimplyFlat->BuildFont("Arial", 25));

    if (sStorage->GetDefaultFontId() < 0)
        RAISE_ERROR("Could not initialize default font!");

    // Here we initialize fonts which come with styles
    // They have to be rendered and saved after OGL init, because of using some of OGL functions to render
    sStorage->BuildStyleFonts();

    // Here we have to load all resources
    sStorage->LoadImageResources();

    srand((unsigned int)(time(NULL)));

    return true;
}

void PresentationMgr::InterfaceEvent(InterfaceEventTypes type, int32 param1, int32 param2)
{
    switch (type)
    {
        // Event for pressing key
        // fires at every key (key == 0) or specified key
        case IE_KEYBOARD_PRESS:
            if (m_slideElement && m_slideElement->elemType == SLIDE_ELEM_KEYBOARD_EVENT &&
                (m_slideElement->typeKeyboardEvent.key == 0 || m_slideElement->typeKeyboardEvent.key == param1) &&
                m_slideElement->typeKeyboardEvent.type == KEYBOARD_EVENT_KEY_DOWN)
                SetBlocking(false);
            break;
        // Event for releasing key
        // fires at every key (key == 0) or specified key
        case IE_KEYBOARD_RELEASE:
            if (m_slideElement && m_slideElement->elemType == SLIDE_ELEM_KEYBOARD_EVENT &&
                (m_slideElement->typeKeyboardEvent.key == 0 || m_slideElement->typeKeyboardEvent.key == param1) &&
                m_slideElement->typeKeyboardEvent.type == KEYBOARD_EVENT_KEY_UP)
                SetBlocking(false);
            break;
        // Event for left mouse button press
        // fires when clicked to specified area, or everywhere when no area specified (coords in line)
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
        // Event for right mouse button press
        // same rules as above
        case IE_MOUSE_RIGHT_DOWN:
            // Verifies if blocking event is mouse event for left button down,
            // if click position is equal to 0 (line) or if clicked point is within the click position
            if (m_slideElement && m_slideElement->elemType == SLIDE_ELEM_MOUSE_EVENT &&
                m_slideElement->typeMouseEvent.type == MOUSE_EVENT_RIGHT_DOWN &&
                ((m_slideElement->typeMouseEvent.positionSquareLU[0] == 0 && m_slideElement->typeMouseEvent.positionSquareRL[0] == 0) ||
                 IN_SQUARE(param1, param2, m_slideElement->typeMouseEvent.positionSquareLU[0],
                    m_slideElement->typeMouseEvent.positionSquareLU[1],
                    m_slideElement->typeMouseEvent.positionSquareRL[0],
                    m_slideElement->typeMouseEvent.positionSquareRL[1])))
                SetBlocking(false);
            break;
        // Event for effect ending
        // fires at effect end, when timer reaches maximum value, or other effect condition satisfaction
        case IE_EFFECT_END:
            if (m_slideElement && m_slideElement->myEffect && m_slideElement->myEffect->getEffectProto()->isBlocking)
                SetBlocking(false);
            break;
        default:
            break;
    }
}

SlideElement* PresentationMgr::GetActiveElementById(const wchar_t* id)
{
    if (m_activeElements.empty())
        return NULL;

    for (SlideList::iterator itr = m_activeElements.begin(); itr != m_activeElements.end(); ++itr)
        if (EqualString((*itr)->elemId.c_str(), id))
            return (*itr);

    return NULL;
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

        // SF before draw events
        sSimplyFlat->BeforeDraw();

        // At first, draw battleground and stuff
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
        {
            // "drawable" parameter is set when building slide element prototype
            if ((*itr)->drawable)
                (*itr)->Draw();
        }

        // SF after draw events
        sSimplyFlat->AfterDraw();

        // If something blocked our presentation, let's wait for some event to unblock it. It should be unblocked in PresentationMgr::InterfaceEvent
        if (m_blocking)
            continue;

        // We are ready to move on
        tmp = sStorage->GetSlideElement(m_slideElementPos);
        if (!tmp)
            break;

        // We have to copy the element from prototype to active element, which we would draw
        // This is due to future support for instancing elements and duplicating them - we would like to take the prototype and just copy it
        m_slideElement = new SlideElement;
        memcpy(m_slideElement, tmp, sizeof(SlideElement));
        tmp = NULL;

        // Effect creation
        // If effect is blocking, then block presentation from any other actions until effect ends
        m_slideElement->CreateEffectIfAny();
        if (m_slideElement->myEffect && m_slideElement->myEffect->getEffectProto()->isBlocking)
            m_blocking = true;

        // And again - if element is drawable (or have some other reason for being stored for future handling), then we have to store it
        if (m_slideElement->drawable)
            m_activeElements.push_back(m_slideElement);

        // Special actions for some element types
        switch (m_slideElement->elemType)
        {
            // Both mouse and keyboard events are blocking
            case SLIDE_ELEM_MOUSE_EVENT:
            case SLIDE_ELEM_KEYBOARD_EVENT:
                m_blocking = true;
                break;
            // Background slide element is also neccessary for handling here, we have to set the BG stuff before drawing another else
            case SLIDE_ELEM_BACKGROUND:
                if (m_slideElement->typeBackground.imageResourceId > 0)
                    bgData.resourceId = m_slideElement->typeBackground.imageResourceId;

                bgData.color = m_slideElement->typeBackground.color;
                break;
            // We will also handle playing effects here
            case SLIDE_ELEM_PLAY_EFFECT:
            {
                SlideElement* target = GetActiveElementById(m_slideElement->elemId.c_str());
                if (target)
                    target->PlayEffect(m_slideElement->elemEffect.c_str());
                break;
            }
            // New slide stuff is also needed to be handled there, this clears all drawable elements from screen
            case SLIDE_ELEM_NEW_SLIDE:
            {
                for (SlideList::iterator itr = m_activeElements.begin(); itr != m_activeElements.end();)
                {
                    // drawable check for future and some kind of "hidden" effects
                    if ((*itr)->drawable)
                        itr = m_activeElements.erase(itr);
                    else
                        ++itr;
                }
                break;
            }
            default:
                break;
        }

        m_slideElementPos++;
    }

    return true;
}
