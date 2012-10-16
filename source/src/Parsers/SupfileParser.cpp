#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers\SupfileParser.h"

bool SupfileParser::Parse(std::vector<std::wstring>* input)
{
    if (!input)
        return false;

    bool slidefiles = false;
    bool effectfiles = false;
    bool stylefiles = false;

    wchar_t* left = NULL;
    wchar_t* right = NULL;

    for (std::vector<std::wstring>::const_iterator itr = input->begin(); itr != input->end(); ++itr)
    {
        left = LeftSide(itr->c_str(), ' ');
        right = RightSide(itr->c_str(), ' ');

        if (left[0] != '\\')
        {
            if (slidefiles)
            {
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
                if (!f)
                    RAISE_ERROR("SupfileParser: Input slide file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputSlideFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }

            if (effectfiles)
            {
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
                if (!f)
                    RAISE_ERROR("SupfileParser: Input effects file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputEffectsFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }

            if (stylefiles)
            {
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
                if (!f)
                    RAISE_ERROR("SupfileParser: Input styles file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputStyleFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }
        }

        if (slidefiles)
            slidefiles = false;
        if (effectfiles)
            effectfiles = false;
        if (stylefiles)
            stylefiles = false;

        // file version
        if (EqualString(left, L"\\EXCEEDER_SUPFILE_VERSION"))
        {
            if (right && wcslen(right) >= 1)
            {
                bool valid = false;
                for (uint32 i = 0; i < sizeof(SupportedSupfileVersions)/sizeof(const char*); i++)
                {
                    if (EqualString(SupportedSupfileVersions[i], right))
                    {
                        sStorage->SetSupfileVersion(right);
                        valid = true;
                        break;
                    }
                }
                if (!valid)
                    RAISE_ERROR("SupfileParser: Unsupported supfile version '%s'!", right);
            }
            else
                RAISE_ERROR("SupfileParser: Unknown version '%s'!", right?ToMultiByteString(right):"none");
        }
        // screen width
        else if (EqualString(left, L"\\SCREEN_WIDTH"))
        {
            if (IsNumeric(right))
                sStorage->SetScreenWidth(ToInt(right));
            else
                RAISE_ERROR("SupfileParser: Non-numeric value '%s' for screen width", right);
        }
        // screen height
        else if (EqualString(left, L"\\SCREEN_HEIGHT"))
        {
            if (IsNumeric(right))
                sStorage->SetScreenHeight(ToInt(right));
            else
                RAISE_ERROR("SupfileParser: Non-numeric value '%s' for screen height", right);
        }
        // slide files
        else if (EqualString(left, L"\\SLIDES"))
        {
            slidefiles = true;
            continue;
        }
        // effect files
        else if (EqualString(left, L"\\EFFECTS"))
        {
            effectfiles = true;
            continue;
        }
        // style files
        else if (EqualString(left, L"\\STYLES"))
        {
            stylefiles = true;
            continue;
        }
        // end of all
        else if (EqualString(left, L"\\END"))
        {
            return true;
        }
        else
        {
            RAISE_ERROR("SupfileParser: Unrecognized key '%s'", left);
        }
    }

    return true;
}
