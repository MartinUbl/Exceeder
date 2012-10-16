#ifndef EXCDR_RESOURCES_H
#define EXCDR_RESOURCES_H

#include "Global.h"
#include "Storage.h"

enum ResourceTypes
{
    RESOURCE_IMAGE      = 0,
    MAX_RESOURCE
};

struct ImageResourceEntry
{
    uint32 textureId;
    std::wstring originalFilename;
};

struct ResourceEntry
{
    ResourceEntry();

    void Prepare(ResourceTypes type, const wchar_t* name, const wchar_t* path);
    void Load();

    uint32 internalId;
    std::wstring name;

    ResourceTypes type;

    // Pointers to various structures specific to valid resource type
    ImageResourceEntry* image;
};

#endif
