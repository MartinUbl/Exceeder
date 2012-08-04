#include "Global.h"
#include "Application.h"
#include "Log.h"
#include "Storage.h"
#include "Presentation.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    sApplication->Init(lpCmdLine);

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

void Application::Init(const char *cmdline)
{
    std::vector<char> option;
    option.clear();
    for (uint32 i = 0; i <= strlen(cmdline); i++)
    {
        if (cmdline[i] == ' ' || cmdline[i] == '\0')
        {
            // recognize option
            option.push_back('\0');
            const char* opt = CharVectorToString(&option);
            if (opt)
            {
                // parameter is file
                if (opt[0] != '-')
                {
                    // init error file
                    if (char* fpath = ExtractFolderFromPath(opt))
                    {
                        char newpath[256];
                        sprintf(&newpath[0], "%s\\err.log", fpath);
                        sLog->InitErrorFile(newpath);
                    }
                    else
                        sLog->InitErrorFile("./err.log");

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
