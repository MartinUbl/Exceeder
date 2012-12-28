#include "Presentation.h"
#include "Application.h"
#include <ctime>

#ifdef _WIN32
LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return sSimplyFlat->SFWndProc(hWnd, msg, wParam, lParam);
}
#endif

PresentationMgr::PresentationMgr()
{
    m_slideElementPos = 0;
    m_slideElement = NULL;
    m_blocking = false;

    m_btEnabled = false;

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

#ifndef _WIN32
void presentationRun()
{
    sPresentation->Run();
}
#endif

bool PresentationMgr::Init()
{
#ifdef _WIN32
    // Use SimplyFlat framework to initialize everything for us
    if (!sSimplyFlat->CreateMainWindow("Exceeder Presentation", sStorage->GetScreenWidth(), sStorage->GetScreenHeight(),
        32, false, 60, &MyWndProc))
        RAISE_ERROR("Could not initialize main window!");
#else
    if (!sSimplyFlat->CreateMainWindow(sApplication->argc, sApplication->argv, "Exceeder Presentation", sStorage->GetScreenWidth(), sStorage->GetScreenHeight(),
        32, false, 60, &presentationRun))
        RAISE_ERROR("Could not initialize main window!");
#endif

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

    sStorage->PostParseElements();

    // Here we have to load all resources
    sStorage->LoadImageResources();

    srand((unsigned int)(time(NULL)));

    // Initialize bluetooth if requested
    // still Win32 only
    if (sStorage->GetBTInterface() != NULL)
    {
#ifdef _WIN32
        m_btHandle = CreateFile(TEXT(ToMultiByteString(sStorage->GetBTInterface())),
                         GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

        COMMTIMEOUTS commTimeouts;
        memset((void*)&commTimeouts, 0, sizeof(commTimeouts));
        commTimeouts.ReadIntervalTimeout = MAXDWORD;
        commTimeouts.ReadTotalTimeoutMultiplier = 0;
        commTimeouts.ReadTotalTimeoutConstant = 0;
        commTimeouts.WriteTotalTimeoutMultiplier = 0;
        commTimeouts.WriteTotalTimeoutConstant = 0;
        SetCommTimeouts(m_btHandle, &commTimeouts);

        if (m_btHandle != INVALID_HANDLE_VALUE)
        {
            DCB dcbSerialParams = {0};
            dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
            GetCommState(m_btHandle, &dcbSerialParams);

            if (GetCommState(m_btHandle, &dcbSerialParams) != 0)
            {
                dcbSerialParams.BaudRate = CBR_9600;
                dcbSerialParams.ByteSize = 7;
                dcbSerialParams.StopBits = ONESTOPBIT;
                dcbSerialParams.Parity = EVENPARITY;

                if (SetCommState(m_btHandle, &dcbSerialParams) != 0)
                {
                    // all BT stuff is OK
                    m_btEnabled = true;
                }
            }
        }
#endif
    }

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

void PresentationMgr::HandleBluetoothMessage(char* msg, uint8 len)
{
    // commands from device connected via Bluetooth

    // Next element
    if (EqualString(msg, "NEXT"))
    {
        SetBlocking(false);
    }
    // Previous element
    else if (EqualString(msg, "PREV"))
    {
        // TODO: move back
    }
}

SlideElement* PresentationMgr::GetActiveElementById(const wchar_t* id)
{
    if (m_activeElements.empty())
        return NULL;

    for (SlideList::iterator itr = m_activeElements.begin(); itr != m_activeElements.end(); ++itr)
        if (EqualString((*itr)->elemId.c_str(), id, true))
            return (*itr);

    return NULL;
}

void PresentationMgr::Run()
{
#ifdef _WIN32
    MSG msg;
#endif
    SlideElement* tmp;

#ifdef _WIN32
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
                PRESENTATION_BREAK;

            PRESENTATION_CONTINUE;
        }
#endif

        // at first, do bluetooth stuff, if enabled
#ifdef _WIN32
        if (m_btEnabled)
        {
            DWORD bytesRead = 0;
            char recvdata[256]={0};
            if (ReadFile(m_btHandle, recvdata, 255, &bytesRead, NULL))
            {
                if (bytesRead > 0)
                {
                    recvdata[bytesRead + 1] = '\0';
                    HandleBluetoothMessage(&recvdata[0], (uint8)(bytesRead+1));
                }
            }
            else
            {
                m_btEnabled = false;
                CloseHandle(m_btHandle);
            }
        }
#endif

        // SF before draw events
        sSimplyFlat->BeforeDraw();

        // At first, draw battleground and stuff
        sSimplyFlat->Drawing->ClearColor(COLOR_R(bgData.color),COLOR_G(bgData.color),COLOR_B(bgData.color));
        if (bgData.resourceId > 0)
        {
            ResourceEntry* res = sStorage->GetResource(bgData.resourceId);
            if (res && res->image)
                sSimplyFlat->Drawing->DrawRectangle(bgData.backgroundPosition[0], bgData.backgroundPosition[1], bgData.backgroundDimensions[0], bgData.backgroundDimensions[1], 0, res->image->textureId);
        }
        if (bgData.source)
        {
            GradientData** ptr = &bgData.source->typeBackground.gradients[0];
            if (bgData.source->typeBackground.gradientEdges)
            {
                if (ptr[GRAD_TOP])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,0,sStorage->GetScreenWidth(), ptr[GRAD_TOP]->size, ptr[GRAD_TOP]->color | 0xFF, ptr[GRAD_TOP]->color | MAKE_COLOR_RGBA(0,0,0,0), VERT_BOTTOM);
                if (ptr[GRAD_BOTTOM])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,sStorage->GetScreenHeight()-ptr[GRAD_BOTTOM]->size, sStorage->GetScreenWidth(), ptr[GRAD_BOTTOM]->size, ptr[GRAD_BOTTOM]->color | 0xFF, ptr[GRAD_BOTTOM]->color | MAKE_COLOR_RGBA(0,0,0,0), VERT_TOP);
                if (ptr[GRAD_LEFT])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,0, ptr[GRAD_LEFT]->size, sStorage->GetScreenHeight(), ptr[GRAD_LEFT]->color | 0xFF, ptr[GRAD_LEFT]->color | MAKE_COLOR_RGBA(0,0,0,0), VERT_RIGHT);
                if (ptr[GRAD_RIGHT])
                    sSimplyFlat->Drawing->DrawRectangleGradient(sStorage->GetScreenWidth()-ptr[GRAD_RIGHT]->size, 0, ptr[GRAD_RIGHT]->size, sStorage->GetScreenHeight(), ptr[GRAD_RIGHT]->color | 0xFF, ptr[GRAD_RIGHT]->color | MAKE_COLOR_RGBA(0,0,0,0), VERT_LEFT);
            }
            else
            {
                // There should be only one gradient set
                // if there are more than one gradient set, it's users fault and we are going to draw all of them

                if (ptr[GRAD_TOP])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,0,sStorage->GetScreenWidth(), sStorage->GetScreenHeight(), ptr[GRAD_TOP]->color | 0xFF, bgData.color | 0xFF, VERT_BOTTOM);
                if (ptr[GRAD_BOTTOM])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,0,sStorage->GetScreenWidth(), sStorage->GetScreenHeight(), ptr[GRAD_BOTTOM]->color | 0xFF, bgData.color | 0xFF, VERT_TOP);
                if (ptr[GRAD_LEFT])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,0,sStorage->GetScreenWidth(), sStorage->GetScreenHeight(), ptr[GRAD_LEFT]->color | 0xFF, bgData.color | 0xFF, VERT_RIGHT);
                if (ptr[GRAD_RIGHT])
                    sSimplyFlat->Drawing->DrawRectangleGradient(0,0,sStorage->GetScreenWidth(), sStorage->GetScreenHeight(), ptr[GRAD_RIGHT]->color | 0xFF, bgData.color | 0xFF, VERT_LEFT);
            }
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
            PRESENTATION_CONTINUE;

        // We are ready to move on
        tmp = sStorage->GetSlideElement(m_slideElementPos);
        if (!tmp)
            PRESENTATION_BREAK;

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
            {
                bgData.source = m_slideElement;

                if (m_slideElement->typeBackground.imageResourceId > 0)
                    bgData.resourceId = m_slideElement->typeBackground.imageResourceId;

                bgData.color = m_slideElement->typeBackground.color;

                // At first, parse dimensions - they are used as base for position calculation
                for (uint32 i = 0; i <= 1; i++)
                {
                    if (i == 0 && (m_slideElement->typeBackground.spread == SPREAD_BOTH || m_slideElement->typeBackground.spread == SPREAD_WIDTH))
                        bgData.backgroundDimensions[0] = sStorage->GetScreenWidth();
                    else if (i == 1 && (m_slideElement->typeBackground.spread == SPREAD_BOTH || m_slideElement->typeBackground.spread == SPREAD_HEIGHT))
                        bgData.backgroundDimensions[1] = sStorage->GetScreenHeight();
                    else if (m_slideElement->typeBackground.dimensions[i] > 0)
                        bgData.backgroundDimensions[i] = m_slideElement->typeBackground.dimensions[i];
                }

                // Position calculating
                for (uint32 i = 0; i <= 1; i++)
                {
                    if (m_slideElement->typeBackground.position[i] > 0)
                        bgData.backgroundPosition[i] = m_slideElement->typeBackground.position[i];
                    else
                    {
                        if (m_slideElement->typeBackground.position[i] == POS_LEFT)
                            bgData.backgroundPosition[i] = 0;
                        else if (m_slideElement->typeBackground.position[i] == POS_RIGHT)
                            bgData.backgroundPosition[i] = sStorage->GetScreenWidth()-bgData.backgroundDimensions[i];
                        else if (m_slideElement->typeBackground.position[i] == POS_TOP)
                            bgData.backgroundPosition[i] = 0;
                        else if (m_slideElement->typeBackground.position[i] == POS_BOTTOM)
                            bgData.backgroundPosition[i] = sStorage->GetScreenHeight()-bgData.backgroundDimensions[i];
                        else if (m_slideElement->typeBackground.position[i] == POS_CENTER)
                        {
                            if (i == 0)
                                bgData.backgroundPosition[i] = (sStorage->GetScreenWidth()-bgData.backgroundDimensions[i])/2;
                            else
                                bgData.backgroundPosition[i] = (sStorage->GetScreenHeight()-bgData.backgroundDimensions[i])/2;
                        }
                    }
                }

                //

                break;
            }
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

#ifdef _WIN32
    }
#endif
}

int64 PresentationMgr::NumerateExpression(ExpressionTreeElement* expr)
{
    // This function MUSTN'T touch anything in expression tree element supplied as it's a pointer to shared memory

    if (!expr)
        return 0;

    int64 tmpval = 0;

    // if element value is string, we can only return real value as integer in the same tree element
    if (expr->valueType == VT_STRING)
    {
        tmpval = GetElementReferenceValue(expr->value.asString);
        return (tmpval*((expr->polarity)?1:(-1)));
    }
    else if (expr->valueType == VT_INTEGER)
        return (expr->value.asLong*((expr->polarity)?1:(-1)));
    else if (expr->valueType == VT_FLOAT)
        return ((int64)expr->value.asDouble*((expr->polarity)?1:(-1)));

    if (expr->items.empty())
        return 0;

    tmpval = NumerateExpression(expr->items[0]);

    for (uint32 i = 1; i < expr->items.size(); i++)
    {
        switch (expr->operation)
        {
            case OP_ADD:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                    tmpval += expr->items[i]->value.asLong;
                else
                    tmpval += (int64)expr->items[i]->value.asDouble;
                break;
            }
            case OP_MULTIPLY:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                    tmpval *= expr->items[i]->value.asLong;
                else
                    tmpval *= (int64)expr->items[i]->value.asDouble;
                break;
            }
            case OP_DIVIDE:
            {
                if (expr->items[i]->valueType == VT_INTEGER)
                {
                    if (expr->items[i]->value.asLong != 0)
                        tmpval /= expr->items[i]->value.asLong;
                }
                else
                {
                    if (expr->items[i]->value.asDouble != 0)
                        tmpval /= (int64)expr->items[i]->value.asDouble;
                }
                break;
            }
            case OP_MODULO:
            {
                // modulo is applicable only on integer types
                // TODO for future: round floating values if modulo is requested, or throw an error
                if (expr->items[i]->valueType == VT_INTEGER)
                {
                    if (expr->items[i]->value.asLong != 0)
                        tmpval %= expr->items[i]->value.asLong;
                }
                break;
            }
        }
    }

    return tmpval;
}

int64 PresentationMgr::GetElementReferenceValue(wchar_t *input)
{
    if (!input || wcslen(input) == 0)
        return 0;

    wchar_t *left = NULL, *right = NULL;
    left = LeftSide(input, L'.');
    right = RightSide(input, L'.');

    if (!left)
        return 0;

    // parsing non-object value
    if (!right)
    {
        if (EqualString(left, L"width", true))
            return sStorage->GetScreenWidth();
        else if (EqualString(left, L"height", true))
            return sStorage->GetScreenHeight();

        return 0;
    }

    SlideElement* tmp = NULL;
    for (SlideList::iterator itr = m_activeElements.begin(); itr != m_activeElements.end(); ++itr)
    {
        if (EqualString((*itr)->elemId.c_str(), left))
        {
            tmp = (*itr);
            break;
        }
    }

    if (!tmp)
        tmp = sStorage->GetSlideElementById(left);

    if (tmp)
    {
        if (EqualString(right, L"x", true))
            return (int64)tmp->position[0];
        else if (EqualString(right, L"y", true))
            return (int64)tmp->position[1];
    }

    return 0;
}
