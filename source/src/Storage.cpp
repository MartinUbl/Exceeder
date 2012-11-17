#include "Global.h"
#include "Log.h"
#include "Storage.h"
#include "Helpers.h"
#include "Parsers/Parser.h"
#include "Parsers/SupfileParser.h"
#include "Parsers/StyleParser.h"
#include "Parsers/SlideParser.h"
#include "Parsers/EffectParser.h"
#include "Parsers/ResourceParser.h"

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

bool Storage::ReadInputSupfile(const wchar_t *path)
{
    if (!path || wcslen(path) < 1)
        return false;

#ifdef _WIN32
    FILE* sfile = _wfopen(path, L"r, ccs=UTF-8");
#else
    FILE* sfile = fopen(ToMultiByteString(path), "r, ccs=UTF-8");
#endif
    if (!sfile)
        return false;

    m_supfilePath = path;

    // preprocess supfile - we need to exclude non-valid lines like all spaces, empty lines or comments
    std::vector<std::wstring> supfile;
    wchar_t* ln = NULL;
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

    for (std::list<std::wstring>::const_iterator itr = m_styleFiles.begin(); itr != m_styleFiles.end(); ++itr)
    {
        if (!StyleParser::ParseFile((*itr).c_str()))
            return false;
    }

    for (std::list<std::wstring>::const_iterator itr = m_effectsFiles.begin(); itr != m_effectsFiles.end(); ++itr)
    {
        if (!EffectParser::ParseFile((*itr).c_str()))
            return false;
    }

    for (std::list<std::wstring>::const_iterator itr = m_resourceFiles.begin(); itr != m_resourceFiles.end(); ++itr)
    {
        if (!ResourceParser::ParseFile((*itr).c_str()))
            return false;
    }

    for (std::list<std::wstring>::const_iterator itr = m_slideFiles.begin(); itr != m_slideFiles.end(); ++itr)
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
    bool fontMatch = false;

    for (StyleMap::iterator itr = m_styleMap.begin(); itr != m_styleMap.end(); ++itr)
    {
        if (itr->second->fontId == -2)
        {
            // This mechanism determines, if some font with that parameters already exists
            for (std::list<StoredFont>::const_iterator iter = m_fontMap.begin(); iter != m_fontMap.end(); ++iter)
            {
                // If yes, assign its ID to style definition and continue to next style
                if (EqualString(ToUppercase(iter->fontName), ToUppercase(itr->second->fontFamily))
                    && iter->fontSize == (*itr->second->fontSize)
                    && iter->bold == itr->second->bold
                    && iter->italic == itr->second->italic
                    && iter->underline == itr->second->underline
                    && iter->strikeout == itr->second->strikeout)
                {
                    itr->second->fontId = iter->fontId;
                    fontMatch = true;
                    break;
                }
            }
            if (fontMatch)
                continue;

            itr->second->fontId = sSimplyFlat->BuildFont(ToMultiByteString(itr->second->fontFamily), (*(itr->second->fontSize)), (itr->second->bold ? FW_BOLD : 0), itr->second->italic, itr->second->underline, itr->second->strikeout);

            // Save font definition for later use
            StoredFont fnt;
            fnt.fontName  = itr->second->fontFamily;
            fnt.fontSize  = (*itr->second->fontSize);
            fnt.fontId    = itr->second->fontId;
            fnt.bold      = itr->second->bold;
            fnt.italic    = itr->second->italic;
            fnt.underline = itr->second->underline;
            fnt.strikeout = itr->second->strikeout;
            m_fontMap.push_back(fnt);
        }
    }
}

void Storage::PostParseElements()
{
    for (std::list<SlideElement*>::iterator itr = m_postParseList.begin(); itr != m_postParseList.end(); ++itr)
    {
        (*itr)->typeText.outlist = new StyledTextList;
        SlideParser::ParseMarkup((*itr)->typeText.text.c_str(), (*itr)->elemStyle.c_str(), (*itr)->typeText.outlist);
        if ((*itr)->typeText.outlist->size() < 2)
        {
            delete (*itr)->typeText.outlist;
            (*itr)->typeText.outlist = NULL;
        }
    }

    m_postParseList.clear();
}
