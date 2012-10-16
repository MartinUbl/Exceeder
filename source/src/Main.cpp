#include "Global.h"
#include "Application.h"
#include "Log.h"
#include "Storage.h"
#include "Presentation.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    sApplication->Init(ToWideString(lpCmdLine));

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
    for (uint32 i = 0; i <= wcslen(cmdline); i++)
    {
        if (cmdline[i] == ' ' || cmdline[i] == '\0')
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
                    if (wchar_t* fpath = ExtractFolderFromPath(opt))
                    {
                        wchar_t newpath[256];
                        swprintf(&newpath[0], L"%s\\err.log", fpath);
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

    sPresentation->Run();
}
