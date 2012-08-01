#ifndef EXCDR_LOG_H
#define EXCDR_LOG_H

#include "Global.h"
#include "Singleton.h"

class Log
{
    public:
        Log();
        ~Log();

        void InitErrorFile(const char* path);

        void ErrorLog(const char* err, ...);
        const char* GetDateTimeString();

    private:

        FILE* m_errorLog;
};

#define sLog Singleton<Log>::instance()

#define RAISE_ERROR(a, ...) { sLog->ErrorLog(a, __VA_ARGS__); return false; }
#define RAISE_ERROR_NORETVAL(a, ...) { sLog->ErrorLog(a, __VA_ARGS__); return;}

#endif
