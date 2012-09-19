#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Parsers\Parser.h"
#include "Parsers\SupfileParser.h"
#include "Parsers\StyleParser.h"
#include "Parsers\SlideParser.h"
#include "Parsers\EffectParser.h"

Storage::Storage()
{
    // implicit screen resolution
    m_screenWidth = 800;
    m_screenHeight = 600;

    m_defaultFontId = -1;
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
    // there has to be at least one input slide file
    if (m_slideFiles.empty())
        RAISE_ERROR("There are no slide files defined!");

    for (std::list<std::string>::const_iterator itr = m_styleFiles.begin(); itr != m_styleFiles.end(); ++itr)
    {
        if (!StyleParser::ParseFile((*itr).c_str()))
            return false;
    }

    for (std::list<std::string>::const_iterator itr = m_effectsFiles.begin(); itr != m_effectsFiles.end(); ++itr)
    {
        if (!EffectParser::ParseFile((*itr).c_str()))
            return false;
    }

    for (std::list<std::string>::const_iterator itr = m_slideFiles.begin(); itr != m_slideFiles.end(); ++itr)
    {
        if (!SlideParser::ParseFile((*itr).c_str()))
            return false;
    }

    // also if there is no slide elements parsed, exit
    if (m_slideData.empty())
        RAISE_ERROR("There are no slide data for your presentation!");

    return true;
}

void Storage::BuildStyleFonts()
{
    for (StyleMap::iterator itr = m_styleMap.begin(); itr != m_styleMap.end(); ++itr)
    {
        if (itr->second->fontId == -2)
            itr->second->fontId = sSimplyFlat->BuildFont(itr->second->fontFamily, (*(itr->second->fontSize)));
    }
}
