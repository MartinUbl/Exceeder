#ifndef EXCDR_STORAGE_H
#define EXCDR_STORAGE_H

#include "Global.h"
#include "Singleton.h"
#include "Defines/Styles.h"
#include "Defines/Effects.h"
#include "Defines/Slides.h"
#include "Resources.h"

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
                if (EqualString(itr->first, name))
                    return itr->second;
            return NULL;
        }

        void AddNewEffect(const wchar_t* name, Effect* eff)
        {
            if (!name || !eff)
                return;

            m_effectMap[name] = eff;
        }
        Effect* GetEffect(const wchar_t* name)
        {
            for (EffectMap::iterator itr = m_effectMap.begin(); itr != m_effectMap.end(); ++itr)
                if (EqualString(itr->first, name))
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

        bool AddMacro(std::wstring id, std::wstring value)
        {
            if (IsMacroDefined(id.c_str()))
                return false;

            MacroPair tmp = MacroPair::pair(id.c_str(), value.c_str());
            m_macros.push_back(tmp);

            return true;
        }

        const wchar_t* GetMacroValue(std::wstring id)
        {
            for (std::list<MacroPair>::const_iterator itr = m_macros.begin(); itr != m_macros.end(); ++itr)
                if (EqualString((*itr).first.c_str(), id.c_str()))
                    return (*itr).second.c_str();

            return NULL;
        }

        bool IsMacroDefined(std::wstring id)
        {
            for (std::list<MacroPair>::const_iterator itr = m_macros.begin(); itr != m_macros.end(); ++itr)
                if (EqualString((*itr).first.c_str(), id.c_str()))
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

    private:

        typedef std::pair<std::wstring, std::wstring> MacroPair;

        std::wstring m_supfilePath;
        std::list<std::wstring> m_styleFiles;
        std::list<std::wstring> m_effectsFiles;
        std::list<std::wstring> m_slideFiles;
        std::list<std::wstring> m_resourceFiles;

        std::vector<ResourceEntry*> m_resources;

        std::wstring m_supfileVersion;

        uint32 m_screenWidth;
        uint32 m_screenHeight;

        // content
        StyleMap m_styleMap;
        EffectMap m_effectMap;
        SlideElementVector m_slideData;

        // macros
        std::list<MacroPair> m_macros;

        int32 m_defaultFontId;
        std::list<StoredFont> m_fontMap;

        std::list<SlideElement*> m_postParseList;
};

#define sStorage Singleton<Storage>::instance()

#endif
