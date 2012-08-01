#include "Global.h"
#include "Storage.h"
#include "Parsers\Parser.h"
#include "Parsers\SupfileParser.h"
#include "Parsers\StyleParser.h"

Storage::Storage()
{
    // implicit screen resolution
    m_screenWidth = 800;
    m_screenHeight = 600;
}

Storage::~Storage()
{
}

bool Storage::ReadInputSupfile(const char *path)
{
    if (!path || strlen(path) < 1)
        return false;

    FILE* sfile = fopen(path, "r");
    if (!sfile)
        return false;

    m_supfilePath = path;

    // preprocess supfile - we need to exclude non-valid lines like all spaces, empty lines or comments
    std::vector<std::string> supfile;
    char* ln = NULL;
    while ((ln = Parser::ReadLine(sfile)) != NULL)
    {
        if (Parser::PrepareLine(ln))
            supfile.push_back(ln);
    }

    return SupfileParser::Parse(&supfile);
}

bool Storage::ParseInputFiles()
{
    for (std::list<std::string>::const_iterator itr = m_styleFiles.begin(); itr != m_styleFiles.end(); ++itr)
    {
        if (!StyleParser::ParseFile((*itr).c_str()))
            return false;
    }

    return true;
}
