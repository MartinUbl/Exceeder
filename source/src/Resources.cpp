#include "Global.h"
#include "Storage.h"
#include "Resources.h"
#include "Log.h"

ResourceEntry::ResourceEntry()
{
    type = MAX_RESOURCE;
    image = NULL;
}

void ResourceEntry::Prepare(ResourceTypes type, const char* name, const char *path)
{
    if (type == RESOURCE_IMAGE)
    {
        image = new ImageResourceEntry;
        image->originalFilename = path;
    }

    this->type = type;
    this->name = name;
}

void ResourceEntry::Load()
{
    if (type == RESOURCE_IMAGE)
        image->textureId = sSimplyFlat->TextureStorage->LoadTexture(image->originalFilename.c_str(), 0);
}

uint32 Storage::PrepareImageResource(const char* name, const char *path)
{
    ResourceEntry* tmp = new ResourceEntry;
    tmp->Prepare(RESOURCE_IMAGE, name, path);

    uint32 id = m_resources.size();
    if (id == 0)
        id++;

    m_resources.resize(id+1);

    tmp->internalId = id;

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

ResourceEntry* Storage::GetResource(const char* name)
{
    for (std::vector<ResourceEntry*>::iterator itr = m_resources.begin(); itr != m_resources.end(); ++itr)
        if ((*itr) && EqualString((*itr)->name.c_str(), name))
            return (*itr);

    return NULL;
}
