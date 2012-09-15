#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers\StyleParser.h"
#include "Defines\Styles.h"

bool StyleParser::ParseFile(const char *path)
{
    if (!path)
        return false;

    FILE* sfile = fopen(path, "r");
    if (!sfile)
        return false;

    std::vector<std::string> stylefile;
    char* ln = NULL;
    while ((ln = ReadLine(sfile)) != NULL)
    {
        if (PrepareLine(ln))
            stylefile.push_back(ln);
    }

    return Parse(&stylefile);
}

bool StyleParser::Parse(std::vector<std::string> *input)
{
    if (!input)
        return false;

    char* left = NULL;
    char* right = NULL;

    char* stylename = NULL;
    Style* tmp = NULL;

    for (std::vector<std::string>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        left = LeftSide(itr->c_str(), ' ');
        right = RightSide(itr->c_str(), ' ');

        // when parsing style definition
        if (stylename)
        {
            if (!tmp)
                tmp = new Style;

            // font family
            if (EqualString(left, "\\FONT_FAMILY"))
            {
                tmp->fontFamily = right;

                // And set "flag" for generate a new font
                if (tmp->fontId >= 0)
                    tmp->fontId = -2;
            }
            // font size in pixels
            else if (EqualString(left, "\\FONT_SIZE"))
            {
                if (IsNumeric(right))
                    tmp->fontSize = new uint32(ToInt(right));
                else
                    RAISE_ERROR("StyleParser: Non-numeric value '%s' used as font size", (right)?right:"none");

                if (tmp->fontId >= 0)
                    tmp->fontId = -2;
            }
            // font color
            else if (EqualString(left, "\\FONT_COLOR"))
            {
                uint32 dst = 0;
                if (ParseColor(right, &dst))
                    tmp->fontColor = new uint32(dst);
                else
                    RAISE_ERROR("StyleParser: Invalid expression '%s' used as font color", (right)?right:"none");

                if (tmp->fontId >= 0)
                    tmp->fontId = -2;
            }
            else if (EqualString(left, "\\DEF_END"))
            {
                sStorage->AddNewStyle(stylename, tmp);
                stylename = NULL;
                tmp = NULL;
            }

            continue;
        }

        // when not parsing style definition
        // file version
        if (EqualString(left, "\\EXCEEDER_STYLES_FILE_VERSION"))
        {
            //
        }
        // start of definition
        else if (EqualString(left, "\\DEF_BEGIN"))
        {
            stylename = right;
            continue;
        }
        // end
        else if (EqualString(left, "\\END"))
        {
            return true;
        }
        else
        {
            RAISE_ERROR("StyleParser: Unrecognized key '%s'", left);
        }
    }

    return true;
}

bool StyleParser::ParseColor(const char *input, uint32 *dest)
{
    if (!input || !dest)
        return false;

    for (uint32 i = 0; i < sizeof(KnownColors)/sizeof(KnownColor); i++)
    {
        if (EqualString(input, KnownColors[i].name))
        {
            (*dest) = KnownColors[i].color;
            return true;
        }
    }

    return false;
}
