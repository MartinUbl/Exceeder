#ifndef EXCDR_STORAGE_H
#define EXCDR_STORAGE_H

#include "Global.h"
#include "Singleton.h"
#include "Defines/Styles.h"
#include "Defines/Effects.h"
#include "Defines/Slides.h"
#include "Defines/Templates.h"
#include "Resources.h"

#define DEFAULT_FONT_SIZE 24
#define DEFAULT_FONT_FAMILY L"Arial"

struct StoredFont
{
    const wchar_t* fontName;
    uint32 fontSize;

    bool bold;
    bool italic;
    bool underline;
    bool strikeout;

    uint32 fontId;
};

class Storage
{
    public:
        Storage();
        ~Storage();

        bool ReadInputSupfile(const wchar_t* path);
        void AddInputStyleFile(const wchar_t* path)   { m_styleFiles.push_back(path); };
        void AddInputEffectsFile(const wchar_t* path) { m_effectsFiles.push_back(path); };
        void AddInputSlideFile(const wchar_t* path)   { m_slideFiles.push_back(path); };
        void AddInputResourceFile(const wchar_t* path){ m_resourceFiles.push_back(path); };
        void AddInputTemplateFile(const wchar_t* path){ m_templateFiles.push_back(path); };
        bool ParseInputFiles();

        void SetScreenWidth(uint32 width) { m_screenWidth = width; };
        void SetScreenHeight(uint32 height) { m_screenHeight = height; };
        uint32 GetScreenWidth() { return m_screenWidth; };
        uint32 GetScreenHeight() { return m_screenHeight; };

        void SetSupfileVersion(std::wstring ver) { m_supfileVersion = ver.c_str(); };
        const wchar_t* GetSupfileVersion() { return m_supfileVersion.c_str(); };
        const wchar_t* GetSupfilePath() { return m_supfilePath.c_str(); };

        void SetDefaultFontId(int32 id) { m_defaultFontId = id; };
        int32 GetDefaultFontId() { return m_defaultFontId; };
        void BuildStyleFonts();

        void AddNewStyle(const wchar_t* name, Style* style)
        {
            if (!name || !style)
                return;

            m_styleMap[name] = style;
        }
        Style* GetStyle(const wchar_t* name)
        {
            for (StyleMap::iterator itr = m_styleMap.begin(); itr != m_styleMap.end(); ++itr)
                if (EqualString(itr->first, name, true))
                    return itr->second;
            return NULL;
        }
        Style* GetDefaultStyle()
        {
            return m_defaultTextStyle;
        }
        void SetupDefaultStyle();
        void SetDefaultStyleName(const wchar_t* name);

        void AddNewEffect(const wchar_t* name, Effect* eff)
        {
            if (!name || !eff)
                return;

            m_effectMap[name] = eff;
        }
        Effect* GetEffect(const wchar_t* name)
        {
            for (EffectMap::iterator itr = m_effectMap.begin(); itr != m_effectMap.end(); ++itr)
                if (EqualString(itr->first, name, true))
                    return itr->second;
            return NULL;
        }

        void AddNewTemplate(const wchar_t* name, SlideTemplate* st)
        {
            if (!name || !st)
                return;

            m_templateMap[name] = st;
        }
        SlideTemplate* GetSlideTemplate(const wchar_t* name)
        {
            for (TemplateMap::iterator itr = m_templateMap.begin(); itr != m_templateMap.end(); ++itr)
                if (EqualString(itr->first, name, true))
                    return itr->second;
            return NULL;
        }

        void AddSlideElement(SlideElement* elem)
        {
            m_slideData.push_back(elem);
        }
        SlideElement* GetSlideElement(uint32 pos)
        {
            if (m_slideData.size() <= pos)
                return NULL;

            return m_slideData[pos];
        }
        SlideElement* GetSlideElementById(std::wstring id)
        {
            for (SlideElementVector::iterator itr = m_slideData.begin(); itr != m_slideData.end(); ++itr)
            {
                if (EqualString((*itr)->elemId, id.c_str()) && (*itr)->elemType != SLIDE_ELEM_PLAY_EFFECT)
                    return (*itr);
            }
            return NULL;
        }
        SlideElement* GetTemplateSlideElementById(std::wstring id)
        {
            // for templates, we need to look up from the bottom, to replace the last inserted element, not first
            for (SlideElementVector::reverse_iterator itr = m_slideData.rbegin(); itr != m_slideData.rend(); ++itr)
            {
                if (EqualString((*itr)->elemId, id.c_str()))
                    return (*itr);
            }
            return NULL;
        }

        bool IsSlideElementBlocking(SlideElement* src, bool staticOnly = false);

        bool AddMacro(std::wstring id, std::wstring value)
        {
            if (IsMacroDefined(id.c_str()))
                return false;

            MacroPair tmp = std::make_pair(id.c_str(), value.c_str());
            m_macros.push_back(tmp);

            return true;
        }

        const wchar_t* GetMacroValue(std::wstring id)
        {
            for (std::list<MacroPair>::const_iterator itr = m_macros.begin(); itr != m_macros.end(); ++itr)
                if (EqualString((*itr).first.c_str(), id.c_str(), true))
                    return (*itr).second.c_str();

            return NULL;
        }

        bool IsMacroDefined(std::wstring id)
        {
            for (std::list<MacroPair>::const_iterator itr = m_macros.begin(); itr != m_macros.end(); ++itr)
                if (EqualString((*itr).first.c_str(), id.c_str(), true))
                    return true;

            return false;
        }

        void AddPostParseElement(SlideElement* elem)
        {
            m_postParseList.push_back(elem);
        }
        void PostParseElements();

        // Resources.cpp
        uint32 PrepareResource(const wchar_t* name, ResourceEntry* res);
        uint32 PrepareImageResource(const wchar_t* name, const wchar_t* path);
        void LoadImageResources();
        ResourceEntry* GetResource(uint32 id);
        ResourceEntry* GetResource(const wchar_t* name);

        void SetBTInterface(const wchar_t* iface)
        {
            m_btInterface = iface;
        }
        const wchar_t* GetBTInterface()
        {
            return m_btInterface;
        }

    private:

        typedef std::pair<std::wstring, std::wstring> MacroPair;

        std::wstring m_supfilePath;
        std::list<std::wstring> m_styleFiles;
        std::list<std::wstring> m_effectsFiles;
        std::list<std::wstring> m_slideFiles;
        std::list<std::wstring> m_resourceFiles;
        std::list<std::wstring> m_templateFiles;

        std::vector<ResourceEntry*> m_resources;

        std::wstring m_supfileVersion;

        const wchar_t* m_btInterface;

        uint32 m_screenWidth;
        uint32 m_screenHeight;

        // content
        StyleMap m_styleMap;
        EffectMap m_effectMap;
        TemplateMap m_templateMap;
        SlideElementVector m_slideData;

        // macros
        std::list<MacroPair> m_macros;

        int32 m_defaultFontId;
        std::list<StoredFont> m_fontMap;
        Style* m_defaultTextStyle;
        std::wstring m_defaultStyleName;

        std::list<SlideElement*> m_postParseList;
};

#define sStorage Singleton<Storage>::instance()

#endif
