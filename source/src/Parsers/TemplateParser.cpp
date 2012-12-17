#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/TemplateParser.h"
#include "Parsers/SlideParser.h"
#include "Defines/Templates.h"

bool TemplateParser::ParseFile(const wchar_t *path)
{
    if (!path)
        return false;

#ifdef _WIN32
    FILE* tfile = _wfopen(path, L"r, ccs=UTF-8");
#else
    FILE* tfile = fopen(ToMultiByteString(path), "r, ccs=UTF-8");
#endif
    if (!tfile)
        return false;

    std::vector<std::wstring> templfile;
    wchar_t* ln = NULL;
    while ((ln = ReadLine(tfile)) != NULL)
    {
        if (PrepareLine(ln))
            templfile.push_back(ln);
    }

    return Parse(&templfile);
}

bool TemplateParser::Parse(std::vector<std::wstring> *input)
{
    if (!input)
        return false;

    wchar_t* left = NULL;
    wchar_t* right = NULL;

    wchar_t* tname = NULL;
    SlideTemplate* tmp = NULL;

    uint8 special;

    for (std::vector<std::wstring>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        left = LeftSide(itr->c_str(), ' ');
        right = RightSide(itr->c_str(), ' ');

        // when parsing template definition
        if (tname)
        {
            if (!tmp)
                tmp = new SlideTemplate;

            // At first, we need to check, if it's not the end of template def
            if (EqualString(left, L"\\TEMPLATE_END", true))
            {
                sStorage->AddNewTemplate(tname, tmp);

                tname = NULL;
                tmp = NULL;
            }
            // if not, parse it as slide element
            else
            {
                SlideElement* el = SlideParser::ParseElement(itr->c_str(), &special);
                if (el && special != SEPF_NON_TEMPLATE)
                    tmp->m_elements.push_back(el);
            }

            continue;
        }

        // when not parsing template definition
        // file version
        if (EqualString(left, L"\\EXCEEDER_TEMPLATES_FILE_VERSION", true))
        {
            //
        }
        // start of definition
        else if (EqualString(left, L"\\TEMPLATE_BEGIN", true))
        {
            tname = right;
            continue;
        }
        // end
        else if (EqualString(left, L"\\END", true))
        {
            return true;
        }
        else
        {
            RAISE_ERROR("TemplateParser: Unrecognized key '%s'", left);
        }
    }

    return true;
}
