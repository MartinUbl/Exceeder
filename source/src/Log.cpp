#include "Global.h"
#include "Log.h"

#include <cstdarg>
#include <time.h>

Log::Log()
{
    m_errorLog = NULL;
}

Log::~Log()
{
    if (m_errorLog)
        fclose(m_errorLog);
}

void Log::InitErrorFile(const wchar_t *path)
{
    if (m_errorLog)
        fclose(m_errorLog);

#ifdef _WIN32
    m_errorLog = _wfopen(path, L"w");
#else
    m_errorLog = fopen(ToMultiByteString(path), "w");
#endif
}

void Log::ErrorLog(const char *err, ...)
{
    va_list argList;
    va_start(argList,err);
    char buf[2048];
    vsnprintf(buf,2048,err,argList);
    va_end(argList);

    cerr << buf << endl;

    // Remove line ending from formatted string
    char* p = (char*)GetDateTimeString();
    p[strlen(p)-1] = '\0';

    if (m_errorLog)
        fprintf(m_errorLog, "%s: %s\n", p, buf);
}

const char* Log::GetDateTimeString()
{
    time_t rtime;
    struct tm* tinfo;

    rtime = time(NULL);
    tinfo = localtime(&rtime);

    return asctime(tinfo);
}
