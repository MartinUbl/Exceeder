#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers\SlideParser.h"
#include "Parsers\StyleParser.h"
#include "Defines\Slides.h"

bool SlideParser::ParseFile(const char *path)
{
    if (!path)
        return false;

    FILE* sfile = fopen(path, "r");
    if (!sfile)
        return false;

    std::vector<std::string> slidefile;
    char* ln = NULL;
    while ((ln = ReadLine(sfile)) != NULL)
    {
        if (PrepareLine(ln))
            slidefile.push_back(ln);
    }

    return Parse(&slidefile);
}

bool SlideParser::Parse(std::vector<std::string> *input)
{
    if (!input)
        return false;

    char* left = NULL;
    char* right = NULL;
    char* middle = NULL; // for additional element definitions

    SlideElement* tmp = NULL;

    uint32 len = 0;

    ParsedDefs defs;

    for (std::vector<std::string>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        defs.clear();

        middle = LeftSide(itr->c_str(), ' ');
        len = strlen(middle);
        // we have to try it again with { delimiter, because of additional definitions of elements
        left = LeftSide(middle, '{');
        if (strlen(left) != strlen(middle))
            middle = RightSide(middle, '{');

        right = RightSide(itr->c_str(), ' ');

        // file version
        if (EqualString(left, "\\EXCEEDER_SLIDE_FILE_VERSION"))
        {
            //
        }
        // background element
        else if (EqualString(left, "\\BACKGROUND"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_BACKGROUND;

            if (!StyleParser::ParseColor(right, &tmp->typeBackground.color))
                RAISE_ERROR("SlideParser: Invalid expression '%s' used as background color", (right)?right:"none");

            tmp->typeBackground.imageResourceId = 0;
            // TODO: make resource system work

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // text element
        else if (EqualString(left, "\\TEXT"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_TEXT;

            ParseInputDefinitions(middle, &defs);

            tmp->elemId = GetDefinitionKeyValue(&defs, "ID");
            tmp->elemStyle = GetDefinitionKeyValue(&defs, "S");
            tmp->elemEffect = GetDefinitionKeyValue(&defs, "E");

            tmp->typeText.text = right;

            GetPositionDefinitionKeyValue(&defs, "P", &tmp->position[0], &tmp->position[1]);

            tmp->typeText.depth = 0;
            if (const char* depth = GetDefinitionKeyValue(&defs, "D"))
                if (IsNumeric(depth))
                    tmp->typeText.depth = ToInt(depth);

            sStorage->AddSlideElement(tmp);

            tmp = NULL;
            continue;
        }
        // mouse press event element
        else if (EqualString(left, "\\MOUSE_LEFT") || EqualString(left, "\\MOUSE_RIGHT"))
        {
            tmp = new SlideElement;
            tmp->elemType = SLIDE_ELEM_MOUSE_EVENT;

            if (EqualString(left, "\\MOUSE_LEFT"))
                tmp->typeMouseEvent.type = MOUSE_EVENT_LEFT_DOWN;
            else
                tmp->typeMouseEvent.type = MOUSE_EVENT_RIGHT_DOWN;

            for (uint32 i = 0; i < 2; i++)
            {
                tmp->typeMouseEvent.positionSquareLU[i] = 0;
                tmp->typeMouseEvent.positionSquareRL[i] = 0;
            }

            ParseInputDefinitions(middle, &defs);

            GetPositionDefinitionKeyValue(&defs, "PLU", &tmp->typeMouseEvent.positionSquareLU[0], &tmp->typeMouseEvent.positionSquareLU[1]);
            GetPositionDefinitionKeyValue(&defs, "PRL", &tmp->typeMouseEvent.positionSquareRL[0], &tmp->typeMouseEvent.positionSquareRL[1]);

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // keyboard press event element
        else if (EqualString(left, "\\KEY_PRESS") || EqualString(left, "\\KEY_RELEASE"))
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
            if (EqualString(left, "\\KEY_PRESS"))
                tmp->typeKeyboardEvent.type = KEYBOARD_EVENT_KEY_DOWN;
            else
                tmp->typeKeyboardEvent.type = KEYBOARD_EVENT_KEY_UP;
            tmp->typeKeyboardEvent.key = key;

            sStorage->AddSlideElement(tmp);

            tmp = NULL;

            continue;
        }
        // end
        else if (EqualString(left, "\\END"))
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

uint16 SlideParser::ResolveKey(const char* input)
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
