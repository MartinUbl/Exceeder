#ifndef EXCDR_RESOURCES_H
#define EXCDR_RESOURCES_H

#include "Global.h"
#include "Storage.h"

enum ResourceTypes
{
    RESOURCE_IMAGE      = 0,
    MAX_RESOURCE
};

enum ImageColorPalette
{
    ICP_FULL            = 0,
    ICP_GRAYSCALE       = 1,
};

struct ImageResourceEntry
{
    uint32 textureId;
    ImageColorPalette colors;
    uint32 colorOverlay;
};

struct ResourceEntry
{
    ResourceEntry();

    void Prepare(ResourceTypes type, const wchar_t* name, const wchar_t* path);
    void Load();

    uint32 internalId;
    std::wstring name;

    ResourceTypes type;

    int32 implicitWidth;
    int32 implicitHeight;

    // filename or other specifications
    std::wstring originalSource;

    std::wstring copyright;
    std::wstring description;

    // Pointers to various structures specific to valid resource type
    ImageResourceEntry* image;
};

#endif
