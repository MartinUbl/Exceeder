#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/SlideParser.h"
#include "Parsers/StyleParser.h"
#include "Defines/Slides.h"

bool SlideParser::ParseFile(const wchar_t *path)
{
    if (!path)
        return false;

#ifdef _WIN32
    FILE* sfile = _wfopen(path, L"r, ccs=UTF-8");
#else
    FILE* sfile = fopen(ToMultiByteString(path), "r, ccs=UTF-8");
#endif
    if (!sfile)
        return false;

    std::vector<std::wstring> slidefile;
    wchar_t* ln = NULL;
    while ((ln = ReadLine(sfile)) != NULL)
    {
        if (PrepareLine(ln) && PreParseLine(ln))
            slidefile.push_back(ln);
    }

    return Parse(&slidefile);
}

bool SlideParser::Parse(std::vector<std::wstring> *input)
{
    if (!input)
        return false;

    wchar_t* left = NULL;
    wchar_t* right = NULL;
    wchar_t* middle = NULL; // for additional element definitions

    SlideElement* tmp = NULL;

    uint32 len = 0;

    ParsedDefs defs;

    for (std::vector<std::wstring>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        defs.clear();

        middle = LeftSide(itr->c_str(), ' ');
        len = wcslen(middle);
        // we have to try it again with { delimiter, because of additional definitions of elements
        left = LeftSide(middle, '{');
        if (wcslen(left) != wcslen(middle))
            middle = RightSide(middle, '{');

        right = RightSide(itr->c_str(), ' ');

        // file version
        if (EqualString(left, L"\\EXCEEDER_SLIDE_FILE_VERSION"))
        {
            //
        }
        // background element
        else if (EqualString(left, L"\\BACKGROUND"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_BACKGROUND;

            if (!StyleParser::ParseColor(right, &tmp->typeBackground.color))
                RAISE_ERROR("SlideParser: Invalid expression '%s' used as background color", (right)?ToMultiByteString(right):"none");

            tmp->typeBackground.imageResourceId = 0;
            // TODO: make resource system work

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // text element
        else if (EqualString(left, L"\\TEXT"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_TEXT;
            tmp->drawable = true;

            ParseInputDefinitions(middle, &defs);

            tmp->elemId = GetDefinitionKeyValue(&defs, L"ID");
            tmp->elemStyle = GetDefinitionKeyValue(&defs, L"S");
            tmp->elemEffect = GetDefinitionKeyValue(&defs, L"E");

            tmp->typeText.text = right;

            GetPositionDefinitionKeyValue(&defs, L"P", &tmp->position[0], &tmp->position[1]);
            tmp->finalPosition[0] = tmp->position[0];
            tmp->finalPosition[1] = tmp->position[1];

            tmp->typeText.depth = 0;
            if (const wchar_t* depth = GetDefinitionKeyValue(&defs, L"D"))
                if (IsNumeric(depth))
                    tmp->typeText.depth = ToInt(depth);

            tmp->typeText.wrapSign = WW_PREWRAP;

            sStorage->AddSlideElement(tmp);

            // Text elements needs to be postparsed due to possibility of marking
            sStorage->AddPostParseElement(tmp);

            tmp = NULL;
            continue;
        }
        // loading an external image
        else if (EqualString(left, L"\\LOAD_IMAGE"))
        {
            std::wstring name, path;

            name = RemoveBeginningSpaces(LeftSide(right, ','));
            path = RemoveBeginningSpaces(RightSide(right, ','));

            if (name.size() < 2)
                RAISE_ERROR("SlideParser: Invalid name %s - name must have at least 2 characters", name.c_str());
            if (path.size() < 1)
                RAISE_ERROR("SlideParser: Invalid path - not entered or not valid");

            sStorage->PrepareImageResource(name.c_str(), path.c_str());

            continue;
        }
        // drawing loaded image
        else if (EqualString(left, L"\\DRAW_IMAGE"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_IMAGE;
            tmp->drawable = true;

            defs.clear();
            ParseInputDefinitions(middle, &defs);

            tmp->elemId = GetDefinitionKeyValue(&defs, L"ID");
            tmp->elemStyle = GetDefinitionKeyValue(&defs, L"S");
            tmp->elemEffect = GetDefinitionKeyValue(&defs, L"E");

            GetPositionDefinitionKeyValue(&defs, L"P", &tmp->position[0], &tmp->position[1]);
            GetPositionDefinitionKeyValue(&defs, L"V", (int32*)&tmp->typeImage.size[0], (int32*)&tmp->typeImage.size[1]); // we can make explicit conversion to int32* since range won't exceed

            ResourceEntry* res = sStorage->GetResource(right);
            if (res)
                tmp->typeImage.resourceId = res->internalId;
            else
                tmp->typeImage.resourceId = 0;

            if (tmp->typeImage.size[0] == 0 || tmp->typeImage.size[1] == 0)
            {
                if (res && res->implicitWidth > 0 && res->implicitHeight > 0)
                {
                    tmp->typeImage.size[0] = res->implicitWidth;
                    tmp->typeImage.size[1] = res->implicitHeight;
                }
                else
                    sLog->ErrorLog("SlideParser: no valid dimensions for resource 's' specified!", right);
            }

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // mouse press event element
        else if (EqualString(left, L"\\MOUSE_LEFT") || EqualString(left, L"\\MOUSE_RIGHT"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_MOUSE_EVENT;

            if (EqualString(left, L"\\MOUSE_LEFT"))
                tmp->typeMouseEvent.type = MOUSE_EVENT_LEFT_DOWN;
            else
                tmp->typeMouseEvent.type = MOUSE_EVENT_RIGHT_DOWN;

            for (uint32 i = 0; i < 2; i++)
            {
                tmp->typeMouseEvent.positionSquareLU[i] = 0;
                tmp->typeMouseEvent.positionSquareRL[i] = 0;
            }

            ParseInputDefinitions(middle, &defs);

            GetPositionDefinitionKeyValue(&defs, L"PLU", (int32*)&tmp->typeMouseEvent.positionSquareLU[0], (int32*)&tmp->typeMouseEvent.positionSquareLU[1]);
            GetPositionDefinitionKeyValue(&defs, L"PRL", (int32*)&tmp->typeMouseEvent.positionSquareRL[0], (int32*)&tmp->typeMouseEvent.positionSquareRL[1]);

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // keyboard press event element
        else if (EqualString(left, L"\\KEY_PRESS") || EqualString(left, L"\\KEY_RELEASE"))
        {
            uint16 key = 0;
            if (right != NULL)
            {
                if (IsNumeric(right))
                    key = ToInt(right);
                else
                    key = ResolveKey(right);
            }

            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_KEYBOARD_EVENT;
            if (EqualString(left, L"\\KEY_PRESS"))
                tmp->typeKeyboardEvent.type = KEYBOARD_EVENT_KEY_DOWN;
            else
                tmp->typeKeyboardEvent.type = KEYBOARD_EVENT_KEY_UP;
            tmp->typeKeyboardEvent.key = key;

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // clear everything from screen
        else if (EqualString(left, L"\\NEW_SLIDE"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_NEW_SLIDE;

            sStorage->AddSlideElement(tmp);

            tmp = NULL;
            continue;
        }
        else if (EqualString(left, L"\\PLAY_EFFECT"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_PLAY_EFFECT;

            ParseInputDefinitions(middle, &defs);

            tmp->elemId = GetDefinitionKeyValue(&defs, L"ID");
            tmp->elemEffect = GetDefinitionKeyValue(&defs, L"E");

            sStorage->AddSlideElement(tmp);

            tmp = NULL;
        }
        // end
        else if (EqualString(left, L"\\END"))
        {
            return true;
        }
        else
        {
            RAISE_ERROR("SlideParser: Unrecognized key '%s'", left);
        }
    }

    return true;
}

uint16 SlideParser::ResolveKey(const wchar_t* input)
{
    if (!input)
        return 0;

    for (uint32 i = 0; i < sizeof(KnownKeys)/sizeof(KnownKey); i++)
    {
        if (EqualString(input, KnownKeys[i].name))
            return KnownKeys[i].code;
    }

    return 0;
}

void SlideParser::ParseMarkup(const wchar_t *input, const wchar_t* stylename, StyledTextList *target)
{
    if (!input)
        return;

    Style* defstyle = NULL;
    if (stylename)
        defstyle = sStorage->GetStyle(stylename);
    else
    {
        defstyle = new Style;
        memset(defstyle, 0, sizeof(Style));
        defstyle->fontId = sStorage->GetDefaultFontId();
        defstyle->fontSize = new uint32(/*DEFAULT_FONT_SIZE*/24);
        defstyle->fontFamily = /*DEFAULT_FONT_FAMILY*/L"Arial";
    }

    Style* origstyle = defstyle;

    printTextData* tmp = NULL;
    wchar_t ident = '\0';

    // valid markups: {B}, {I}, {U}, {X}, {S:style_name}
    uint32 len = wcslen(input);

    uint32 lastTextBegin = 0;

    uint32 i;
    for (i = 0; i < len; )
    {
        // opening tag (without '/' at second position)
        if (i < len-2 && input[i] == L'{' && input[i+1] != L'/' && (input[i+2] == L'}' || input[i+2] == L':'))
        {
            ident = input[i+1];
            if (ident == L'B' || ident == L'I' || ident == L'U' || ident == L'X' || (ident == L'S' && input[i+2] == L':'))
            {
                if (!target)
                    target = new StyledTextList;

                tmp = new printTextData;

                tmp->fontId = defstyle->fontId;

                tmp->feature = SlideElement::elemTextData::GetFeatureArrayIndexOf(defstyle);
                tmp->colorize = !(defstyle->fontColor == NULL);
                if (tmp->colorize)
                    tmp->color = (*(defstyle->fontColor));

                tmp->text = new wchar_t[i-lastTextBegin+1];
                memset(tmp->text, 0, sizeof(wchar_t)*(i-lastTextBegin+1));
                wcsncpy(tmp->text, &(input[lastTextBegin]), i-lastTextBegin);

                target->push_back(tmp);

                if (ident == L'B')
                {
                    defstyle->bold = true;
                    origstyle->bold = true;
                }
                else if (ident == L'I')
                {
                    defstyle->italic = true;
                    origstyle->italic = true;
                }
                else if (ident == L'U')
                {
                    defstyle->underline = true;
                    origstyle->underline = true;
                }
                else if (ident == L'X')
                {
                    defstyle->strikeout = true;
                    origstyle->strikeout = true;
                }
                else if (ident == L'S')
                {
                    const wchar_t* stname = LeftSide(&(input[i+3]), L'}');
                    if (stname)
                        defstyle = sStorage->GetStyle(stname);

                    i += 3 + 1 + wcslen(stname);
                }

                if (ident != L'S')
                    i += 3;

                lastTextBegin = i;
                continue;
            }
            i++;
            continue;
        }
        // closing tag (with '/' at second position)
        else if (i < len-2 && input[i] == L'{' && input[i+1] == L'/' && input[i+3] == L'}')
        {
            ident = input[i+2];
            if (ident == L'B' || ident == L'I' || ident == L'U' || ident == L'X' || ident == L'S')
            {
                if (!target)
                    target = new StyledTextList;

                tmp = new printTextData;

                tmp->fontId = defstyle->fontId;

                tmp->feature = SlideElement::elemTextData::GetFeatureArrayIndexOf(defstyle);
                tmp->colorize = !(defstyle->fontColor == NULL);
                if (tmp->colorize)
                    tmp->color = (*(defstyle->fontColor));

                tmp->text = new wchar_t[i-lastTextBegin+1];
                memset(tmp->text, 0, sizeof(wchar_t)*(i-lastTextBegin+1));
                wcsncpy(tmp->text, &(input[lastTextBegin]), i-lastTextBegin);

                target->push_back(tmp);

                if (ident == L'B')
                {
                    defstyle->bold = false;
                    origstyle->bold = false;
                }
                else if (ident == L'I')
                {
                    defstyle->italic = false;
                    origstyle->italic = false;
                }
                else if (ident == L'U')
                {
                    defstyle->underline = false;
                    origstyle->underline = false;
                }
                else if (ident == L'X')
                {
                    defstyle->strikeout = false;
                    origstyle->strikeout = false;
                }
                else if (ident == L'S')
                    defstyle = origstyle;

                i += 4;
                lastTextBegin = i;
                continue;
            }
            i++;
            continue;
        }
        i++;
    }

    if (lastTextBegin != i)
    {
        tmp = new printTextData;

        tmp->fontId = defstyle->fontId;

        tmp->feature = SlideElement::elemTextData::GetFeatureArrayIndexOf(defstyle);
        tmp->colorize = !(defstyle->fontColor == NULL);
        if (tmp->colorize)
            tmp->color = (*(defstyle->fontColor));

        tmp->text = new wchar_t[i-lastTextBegin+1];
        memset(tmp->text, 0, sizeof(wchar_t)*(i-lastTextBegin+1));
        wcsncpy(tmp->text, &(input[lastTextBegin]), i-lastTextBegin);

        target->push_back(tmp);
    }
}
