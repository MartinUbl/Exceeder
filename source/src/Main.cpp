#include "Global.h"
#include "Application.h"
#include "Log.h"
#include "Storage.h"
#include "Presentation.h"

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
    setlocale(LC_ALL, "en_US.UTF-8");
#ifdef _WIN32
    sApplication->Init(ToWideString(lpCmdLine));
#else
    if (argc < 2)
        return -1;

    sApplication->argc = new int(argc);
    sApplication->argv = argv;

    wchar_t* line = new wchar_t[1024];
    memset(line, 0, sizeof(wchar_t)*1024);
    swprintf(line, 9999, L"%s", argv[1]);
    for (uint32 i = 2; i < argc; i++)
        swprintf(line, 9999, L"%s %s", line, argv[i]);

    sApplication->Init(line);
#endif

    if (!sApplication->Initialized())
        return -1;

    sApplication->Run();

    return 0;
}

Application::Application()
{
    m_init = false;
}

Application::~Application()
{
}

void Application::Init(const wchar_t *cmdline)
{
    std::vector<wchar_t> option;
    option.clear();

    bool endWithQuotes = false;
    if (cmdline[0] == '\"')
        endWithQuotes = true;

    for (uint32 i = endWithQuotes?1:0; i <= wcslen(cmdline); i++)
    {
        if ((endWithQuotes && cmdline[i] == '\"') || (!endWithQuotes && (cmdline[i] == ' ' || cmdline[i] == '\0')))
        {
            // recognize option
            option.push_back('\0');
            const wchar_t* opt = CharVectorToString(&option);
            if (opt)
            {
                // parameter is file
                if (opt[0] != '-')
                {
                    // init error file
                    wchar_t* fpath = ExtractFolderFromPath(opt);
                    if (fpath)
                    {
#ifdef _WIN32
                        _wchdir(fpath);
#else
                        chdir(ToMultiByteString(fpath));
#endif
                        wchar_t newpath[256];
                        swprintf(&newpath[0], 99999, L"%s\\err.log", fpath);
                        sLog->InitErrorFile(newpath);
                    }
                    else
                        sLog->InitErrorFile(L"./err.log");

                    if (!sStorage->ReadInputSupfile(opt))
                        return;

                    if (!sStorage->ParseInputFiles())
                        return;
                }
            }

            option.clear();
        }
        else
            option.push_back(cmdline[i]);
    }

    m_init = true;
}

void Application::Run()
{
    if (!m_init)
        return;

    if (!sPresentation->Init())
        return;

#ifdef _WIN32
    sPresentation->Run();
#else
    sSimplyFlat->Run();
#endif
}
