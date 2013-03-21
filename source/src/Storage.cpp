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
#include "Parsers/TemplateParser.h"

Storage::Storage()
{
    // implicit screen resolution
    m_screenWidth = 800;
    m_screenHeight = 600;
    m_originalScreenWidth = 800;
    m_originalScreenHeight = 600;
    m_fullscreen = true;

    m_defaultFontId = -1;
    m_defaultTextStyle = NULL;
    m_defaultStyleName = L"";

    m_btInterface = NULL;
    m_networkPort = 0;

    m_lastOverwrittenElement = m_slideData.end();
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

    for (std::list<std::wstring>::const_iterator itr = m_templateFiles.begin(); itr != m_templateFiles.end(); ++itr)
    {
        if (!TemplateParser::ParseFile((*itr).c_str()))
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

bool Storage::IsSlideElementBlocking(SlideElement* src, bool staticOnly)
{
    if (!src)
        return false;

    if (src->elemType == SLIDE_ELEM_BLOCK)
    {
        if (src->typeBlock.passthrough && staticOnly)
            return false;
        else
            return true;
    }

    // certain slide element types are blocking by default
    if (src->elemType == SLIDE_ELEM_MOUSE_EVENT || src->elemType == SLIDE_ELEM_KEYBOARD_EVENT)
        return true;

    // and some elements have only blocking effect on them
    if (!staticOnly && src->elemEffect && wcslen(src->elemEffect) > 0)
    {
        Effect* eff = GetEffect(src->elemEffect);
        if (eff && eff->isBlocking)
            return true;
    }

    return false;
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
                if (EqualString(ToUppercase(iter->fontName), ToUppercase(itr->second->fontFamily), true)
                    && iter->fontSize == (*itr->second->fontSize)
                    && iter->bold == itr->second->bold
                    && iter->italic == itr->second->italic
                    && iter->underline == itr->second->underline
                    && iter->strikeout == itr->second->strikeout)
                {
                    if (iter->fontId >= 0)
                    {
                        itr->second->fontId = iter->fontId;
                        fontMatch = true;
                        break;
                    }
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

    PostParseElements();
}

void Storage::SetupDefaultStyle()
{
    if (m_defaultTextStyle)
        delete m_defaultTextStyle;

    m_defaultTextStyle = NULL;

    if (m_defaultStyleName.size() > 0)
    {
        Style* st = sStorage->GetStyle(m_defaultStyleName.c_str());
        if (st)
        {
            m_defaultTextStyle = new Style;
            memset(m_defaultTextStyle, 0, sizeof(Style));

            if (st->fontColor)
                m_defaultTextStyle->fontColor = new uint32(*st->fontColor);

            m_defaultTextStyle->fontFamily = new wchar_t[wcslen(st->fontFamily)+1];
            memset((void*)m_defaultTextStyle->fontFamily, 0, wcslen(st->fontFamily)+1);
            wcsncpy((wchar_t*)m_defaultTextStyle->fontFamily, st->fontFamily, wcslen(st->fontFamily));

            if (st->fontSize)
                m_defaultTextStyle->fontSize = new uint32(*st->fontSize);

            if (st->overlayColor)
                m_defaultTextStyle->overlayColor = new uint32(*st->overlayColor);

            if (st->fontId)
                m_defaultTextStyle->fontId = st->fontId;
        }
    }

    if (!m_defaultTextStyle)
    {
        m_defaultTextStyle = new Style;
        memset(m_defaultTextStyle, 0, sizeof(Style));

        m_defaultTextStyle->fontId = sStorage->GetDefaultFontId();
        m_defaultTextStyle->fontSize = new uint32(DEFAULT_FONT_SIZE);
        m_defaultTextStyle->fontFamily = DEFAULT_FONT_FAMILY;
        m_defaultTextStyle->fontColor = 0;
    }
}

void Storage::SetDefaultStyleName(const wchar_t* name)
{
    m_defaultStyleName = name;
}

void Storage::PostParseElements()
{
    for (std::list<SlideElement*>::iterator itr = m_postParseList.begin(); itr != m_postParseList.end(); )
    {
        (*itr)->typeText.outlist = new StyledTextList;
        SlideParser::ParseMarkup((*itr)->typeText.text, (*itr)->elemStyle, (*itr)->typeText.outlist, &((*itr)->typeText.outlistExpressions));
        if ((*itr)->typeText.outlist->size() < 2)
        {
            delete (*itr)->typeText.outlist;
            (*itr)->typeText.outlist = NULL;
            ++itr;
            continue;
        }

        itr = m_postParseList.erase(itr);
    }
}
