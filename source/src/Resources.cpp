#include "Global.h"
#include "Storage.h"
#include "Resources.h"
#include "Log.h"

ResourceEntry::ResourceEntry()
{
    type = MAX_RESOURCE;
    image = NULL;
}

void ResourceEntry::Prepare(ResourceTypes type, const wchar_t* name, const wchar_t *path)
{
    if (type == RESOURCE_IMAGE)
    {
        image = new ImageResourceEntry;
        image->colorOverlay = 0;
        image->colors = ICP_FULL;
    }

    this->type = type;
    this->name = name;
}

void ResourceEntry::Load()
{
    if (type == RESOURCE_IMAGE)
    {
        uint32 flags = 0;

        if (image->colors == ICP_GRAYSCALE)
            flags |= IMAGELOAD_GREYSCALE;

        image->textureId = sSimplyFlat->TextureStorage->LoadTexture(ToMultiByteString(originalSource.c_str()), flags);
    }
}

uint32 Storage::PrepareResource(const wchar_t* name, ResourceEntry* res)
{
    if (!res)
        return 0;

    if (res->type == MAX_RESOURCE)
        return 0;

    res->Prepare(res->type, name, res->originalSource.c_str());

    uint32 id = m_resources.size();
    if (id == 0)
        id++;

    m_resources.resize(id+1);

    res->internalId = id;

    m_resources[id] = res;

    return id;
}

uint32 Storage::PrepareImageResource(const wchar_t* name, const wchar_t *path)
{
    ResourceEntry* tmp = new ResourceEntry;
    tmp->Prepare(RESOURCE_IMAGE, name, path);

    uint32 id = m_resources.size();
    if (id == 0)
        id++;

    m_resources.resize(id+1);

    tmp->internalId = id;

    tmp->originalSource = path;

    m_resources[id] = tmp;

    return id;
}

void Storage::LoadImageResources()
{
    for (std::vector<ResourceEntry*>::iterator itr = m_resources.begin(); itr != m_resources.end(); ++itr)
        if (*itr)
            (*itr)->Load();
}

ResourceEntry* Storage::GetResource(uint32 id)
{
    if (m_resources.size() <= id)
        return NULL;

    return m_resources[id];
}

ResourceEntry* Storage::GetResource(const wchar_t* name)
{
    for (std::vector<ResourceEntry*>::iterator itr = m_resources.begin(); itr != m_resources.end(); ++itr)
        if ((*itr) && EqualString((*itr)->name.c_str(), name, true))
            return (*itr);

    return NULL;
}
