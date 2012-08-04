#ifndef EXCDR_STORAGE_H
#define EXCDR_STORAGE_H

#include "Global.h"
#include "Singleton.h"
#include "Defines\Styles.h"
#include "Defines\Slides.h"

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

        void AddNewStyle(const char* name, Style* style)
        {
            if (!name || !style)
                return;

            m_styleMap[name] = style;
        }
        Style* GetStyle(const char* name)
        {
            if (m_styleMap.find(name) != m_styleMap.end())
                return m_styleMap[name];
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
        SlideElementVector m_slideData;
};

#define sStorage Singleton<Storage>::instance()

#endif
