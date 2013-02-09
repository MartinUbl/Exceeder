#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers/SupfileParser.h"

bool SupfileParser::Parse(std::vector<std::wstring>* input)
{
    if (!input)
        return false;

    bool slidefiles = false;
    bool effectfiles = false;
    bool stylefiles = false;
    bool resfiles = false;
    bool templfiles = false;

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
#ifdef _WIN32
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
#else
                FILE* f = fopen(ToMultiByteString(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left)), "r, ccs=UTF-8");
#endif
                if (!f)
                    RAISE_ERROR("SupfileParser: Input slide file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputSlideFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }

            if (effectfiles)
            {
#ifdef _WIN32
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
#else
                FILE* f = fopen(ToMultiByteString(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left)), "r, ccs=UTF-8");
#endif
                if (!f)
                    RAISE_ERROR("SupfileParser: Input effects file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputEffectsFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }

            if (stylefiles)
            {
#ifdef _WIN32
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
#else
                FILE* f = fopen(ToMultiByteString(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left)), "r, ccs=UTF-8");
#endif
                if (!f)
                    RAISE_ERROR("SupfileParser: Input styles file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputStyleFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }

            if (resfiles)
            {
#ifdef _WIN32
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
#else
                FILE* f = fopen(ToMultiByteString(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left)), "r, ccs=UTF-8");
#endif
                if (!f)
                    RAISE_ERROR("SupfileParser: Input resource file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputResourceFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }

            if (templfiles)
            {
#ifdef _WIN32
                FILE* f = _wfopen(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left), L"r, ccs=UTF-8");
#else
                FILE* f = fopen(ToMultiByteString(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left)), "r, ccs=UTF-8");
#endif
                if (!f)
                    RAISE_ERROR("SupfileParser: Input template file '%s', hasn't been found!", ToMultiByteString(left));
                fclose(f);

                sStorage->AddInputTemplateFile(MakeFilePath(ExtractFolderFromPath(sStorage->GetSupfilePath()), left));
                continue;
            }
        }

        if (slidefiles)
            slidefiles = false;
        if (effectfiles)
            effectfiles = false;
        if (stylefiles)
            stylefiles = false;
        if (resfiles)
            resfiles = false;
        if (templfiles)
            templfiles = false;

        // file version
        if (EqualString(left, L"\\EXCEEDER_SUPFILE_VERSION", true))
        {
            if (right && wcslen(right) >= 1)
            {
                bool valid = false;
                for (uint32 i = 0; i < sizeof(SupportedSupfileVersions)/sizeof(const char*); i++)
                {
                    if (EqualString(SupportedSupfileVersions[i], right, true))
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
        else if (EqualString(left, L"\\SCREEN_WIDTH", true))
        {
            if (IsNumeric(right))
                sStorage->SetScreenWidth(ToInt(right));
            else
                RAISE_ERROR("SupfileParser: Non-numeric value '%s' for screen width", right);
        }
        // screen height
        else if (EqualString(left, L"\\SCREEN_HEIGHT", true))
        {
            if (IsNumeric(right))
                sStorage->SetScreenHeight(ToInt(right));
            else
                RAISE_ERROR("SupfileParser: Non-numeric value '%s' for screen height", right);
        }
        // bluetooth interface if needed
        else if (EqualString(left, L"\\BLUETOOTH_INTERFACE", true))
        {
            if (right && wcslen(right) > 0)
                sStorage->SetBTInterface(right);
        }
        // general network access
        else if (EqualString(left, L"\\NETWORK_CONTROL", true))
        {
            if (right && IsNumeric(right))
                sStorage->SetNetworkPort(ToInt(right));
            else if (right)
            {
                sLog->ErrorLog("SupfileParser: Non-numeric value '%S' used as network port. Using default (%u).", right, DEFAULT_NETWORK_PORT);
                sStorage->SetNetworkPort(DEFAULT_NETWORK_PORT);
            }
            else
                sStorage->SetNetworkPort(DEFAULT_NETWORK_PORT);
        }
        // default style setting
        else if (EqualString(left, L"\\DEFAULT_STYLE", true))
        {
            if (right && wcslen(right) > 0)
                sStorage->SetDefaultStyleName(right);
        }
        // slide files
        else if (EqualString(left, L"\\SLIDES", true))
        {
            slidefiles = true;
            continue;
        }
        // effect files
        else if (EqualString(left, L"\\EFFECTS", true))
        {
            effectfiles = true;
            continue;
        }
        // style files
        else if (EqualString(left, L"\\STYLES", true))
        {
            stylefiles = true;
            continue;
        }
        // resource files
        else if (EqualString(left, L"\\RESOURCES", true))
        {
            resfiles = true;
            continue;
        }
        // template files
        else if (EqualString(left, L"\\TEMPLATES", true))
        {
            templfiles = true;
            continue;
        }
        // end of all
        else if (EqualString(left, L"\\END", true))
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
