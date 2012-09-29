#ifndef EXCDR_STORAGE_H
#define EXCDR_STORAGE_H

#include "Global.h"
#include "Singleton.h"
#include "Defines\Styles.h"
#include "Defines\Effects.h"
#include "Defines\Slides.h"

struct StoredFont
{
    const char* fontName;
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

        bool ReadInputSupfile(const char* path);
        void AddInputStyleFile(const char* path)   { m_styleFiles.push_back(path); };
        void AddInputEffectsFile(const char* path) { m_effectsFiles.push_back(path); };
        void AddInputSlideFile(const char* path)   { m_slideFiles.push_back(path); };
        bool ParseInputFiles();

        void SetScreenWidth(uint32 width) { m_screenWidth = width; };
        void SetScreenHeight(uint32 height) { m_screenHeight = height; };
        uint32 GetScreenWidth() { return m_screenWidth; };
        uint32 GetScreenHeight() { return m_screenHeight; };

        void SetSupfileVersion(std::string ver) { m_supfileVersion = ver.c_str(); };
        const char* GetSupfileVersion() { return m_supfileVersion.c_str(); };
        const char* GetSupfilePath() { return m_supfilePath.c_str(); };

        void SetDefaultFontId(int32 id) { m_defaultFontId = id; };
        int32 GetDefaultFontId() { return m_defaultFontId; };
        void BuildStyleFonts();

        void AddNewStyle(const char* name, Style* style)
        {
            if (!name || !style)
                return;

            m_styleMap[name] = style;
        }
        Style* GetStyle(const char* name)
        {
            for (StyleMap::iterator itr = m_styleMap.begin(); itr != m_styleMap.end(); ++itr)
                if (EqualString(itr->first, name))
                    return itr->second;
            return NULL;
        }

        void AddNewEffect(const char* name, Effect* eff)
        {
            if (!name || !eff)
                return;

            m_effectMap[name] = eff;
        }
        Effect* GetEffect(const char* name)
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

    private:

        std::string m_supfilePath;
        std::list<std::string> m_styleFiles;
        std::list<std::string> m_effectsFiles;
        std::list<std::string> m_slideFiles;

        std::string m_supfileVersion;

        uint32 m_screenWidth;
        uint32 m_screenHeight;

        // content
        StyleMap m_styleMap;
        EffectMap m_effectMap;
        SlideElementVector m_slideData;

        int32 m_defaultFontId;
        std::list<StoredFont> m_fontMap;
};

#define sStorage Singleton<Storage>::instance()

#endif
