#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/ResourceParser.h"
#include "Resources.h"

bool ResourceParser::ParseFile(const wchar_t *path)
{
    if (!path)
        return false;

#ifdef _WIN32
    FILE* rfile = _wfopen(path, L"r, ccs=UTF-8");
#else
    FILE* rfile = fopen(ToMultiByteString(path), "r, ccs=UTF-8");
#endif
    if (!rfile)
        return false;

    std::vector<std::wstring> resfile;
    wchar_t* ln = NULL;
    while ((ln = ReadLine(rfile)) != NULL)
    {
        if (PrepareLine(ln))
            resfile.push_back(ln);
    }

    return Parse(&resfile);
}

bool ResourceParser::Parse(std::vector<std::wstring> *input)
{
    if (!input)
        return false;

    wchar_t* left = NULL;
    wchar_t* right = NULL;

    wchar_t* resname = NULL;
    ResourceEntry* tmp = NULL;

    // parameters which needs to be added after insertion to Storage fields
    ImageColorPalette icp = ICP_FULL;

    for (std::vector<std::wstring>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        left = LeftSide(itr->c_str(), ' ');
        right = RightSide(itr->c_str(), ' ');

        // when parsing style definition
        if (resname)
        {
            if (!tmp)
                tmp = new ResourceEntry;

            // font family
            if (EqualString(left, L"\\TYPE"))
            {
                if (EqualString(right, L"image"))
                    tmp->type = RESOURCE_IMAGE;
            }
            else if (EqualString(left, L"\\SOURCE"))
            {
                tmp->originalSource = right;
            }
            else if (EqualString(left, L"\\WIDTH"))
            {
                if (IsNumeric(right))
                    tmp->implicitWidth = ToInt(right);
                else
                    RAISE_ERROR("ResourceParser: non-numeric value '%s' supplied as width for resource '%s'", right, resname);
            }
            else if (EqualString(left, L"\\HEIGHT"))
            {
                if (IsNumeric(right))
                    tmp->implicitHeight = ToInt(right);
                else
                    RAISE_ERROR("ResourceParser: non-numeric value '%s' supplied as height for resource '%s'", right, resname);
            }
            else if (EqualString(left, L"\\COPYRIGHT"))
            {
                tmp->copyright = right;
            }
            else if (EqualString(left, L"\\DESCRIPTION"))
            {
                tmp->description = right;
            }
            else if (EqualString(left, L"\\COLORS"))
            {
                if (EqualString(right, L"full"))
                    icp = ICP_FULL;
                else if (EqualString(right, L"grayscale") || EqualString(right, L"greyscale"))
                    icp = ICP_GRAYSCALE;
                else
                    sLog->ErrorLog("ResourceParser: invalid color palette definition '%s' supplied for resource '%s', using full palette", right, resname);
            }
            else if (EqualString(left, L"\\DEF_END"))
            {
                uint32 resid = 0;
                if (tmp->type != MAX_RESOURCE)
                    resid = sStorage->PrepareResource(resname, tmp);
                else
                    RAISE_ERROR("ResourceParser: invalid input resource specified for resource '%s'", resname);

                // Adding new resource went OK
                if (resid > 0)
                {
                    if (tmp->type == RESOURCE_IMAGE)
                        tmp->image->colors = icp;
                }

                resname = NULL;
                tmp = NULL;
            }

            continue;
        }

        // when not parsing style definition
        // file version
        if (EqualString(left, L"\\EXCEEDER_RESOURCES_FILE_VERSION"))
        {
            //
        }
        // start of definition
        else if (EqualString(left, L"\\DEF_BEGIN"))
        {
            resname = right;
            continue;
        }
        // end
        else if (EqualString(left, L"\\END"))
        {
            return true;
        }
        else
        {
            RAISE_ERROR("ResourceParser: Unrecognized key '%s'", left);
        }
    }

    return true;
}
