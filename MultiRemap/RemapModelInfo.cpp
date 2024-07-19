#include "RemapModelInfo.h"

#include "Log.h"

RemapModelInfo::RemapModelInfo(int modelId, eRemapType type)
{
    this->modelId = modelId;
    this->type = type;
}

bool RemapModelInfo::HasRemap(std::string name)
{
    return remaps.count(name);
}

Remap* RemapModelInfo::TryCreateRemap(std::string name)
{
    if(HasRemap(name))
    {
        return GetRemap(name);
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Creating new Remap: " << name << std::endl;

    auto remap = new Remap();
    remap->name = name;
    
    remaps[name] = remap;

    return remap;
}

Remap* RemapModelInfo::GetRemap(std::string name)
{
    if(!HasRemap(name)) return NULL;

    return remaps[name];
}

Remap* RemapModelInfo::GetRandomRemap()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "GetRandomRemap" << std::endl;

    std::vector<Remap*> vec;

    for(auto p : remaps)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "possible remap: " << p.first << std::endl;
        vec.push_back(p.second);
    }

    int i = GetRandomNumber(0, vec.size() - 1);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "selected i: " << i << ", name: " << vec[i]->name << std::endl;

    return vec[i];
}