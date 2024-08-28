#include "RemapModelInfos.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

#include "iniconfig/INIFile.h"

#include "Log.h"
#include "ModConfig.h"

std::map<int, RemapModelInfo*> RemapModelInfos::m_ModelInfos;

void RemapModelInfos::Load()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "RemapModelInfos: Load" << std::endl;

    auto remapsDir = ModConfig::GetConfigFolder() + "/remaps/";

    if(!ModConfig::DirExists(remapsDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "/remaps folder was empty! Creating one..." << std::endl;
        ModConfig::CreateFolder(remapsDir);
    }

    LoadVehicles();
    LoadSkins();
}

void RemapModelInfos::LoadVehicles()
{
    auto remapsDir = ModConfig::GetConfigFolder() + "/remaps/vehicles/";

    if(!ModConfig::DirExists(remapsDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "/vehicles folder was empty! Creating one..." << std::endl;
        ModConfig::CreateFolder(remapsDir);
    }

    auto remapBaseNames = ModConfig::GetDirectoriesName(remapsDir);
    for(auto remapBaseName : remapBaseNames)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Found remap vehicle: " << remapBaseName << std::endl;
        
        INIFile file;
        if (!file.Read(remapsDir + "/" + remapBaseName + "/config.ini"))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not read config.ini" << std::endl;
            continue;
        }

        auto sections = file.GetSections("VEHICLE");
        if (sections.size() == 0) continue;
        
        auto section = sections[0];
        
        int modelId = section->GetIntWithDefaultValue("vehicle_model_id", 0);

        if (modelId == 0) continue;

        auto modelInfo = RemapModelInfos::AddModelInfo(modelId, eRemapType::REMAP_VEHICLE);

        auto remapNames = ModConfig::GetDirectoriesName(remapsDir + "/" + remapBaseName);
        for(auto remapName : remapNames)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Reading remap: " << remapName << std::endl;

            auto remap = modelInfo->TryCreateRemap(remapName);

            auto files = ModConfig::GetFilesName(remapsDir + "/" + remapBaseName + "/" + remapName);
            for(auto file : files)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Found texture: " << file << std::endl;

                RemapTexure remapTexture;
                remapTexture.textureName = ModConfig::ReplaceExtension(file, "");
                remapTexture.textureFile = remapsDir + "/" + remapBaseName + "/" + remapName + "/" + file;
                remapTexture.texture = NULL;

                remap->remapTextures.push_back(remapTexture);
            }
        }
    }   
}

void RemapModelInfos::LoadSkins()
{
    auto remapsDir = ModConfig::GetConfigFolder() + "/remaps/skins/";

    if(!ModConfig::DirExists(remapsDir))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "/skins folder was empty! Creating one..." << std::endl;
        ModConfig::CreateFolder(remapsDir);
    }

    auto remapBaseNames = ModConfig::GetDirectoriesName(remapsDir);
    for(auto remapBaseName : remapBaseNames)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Found remap skin: " << remapBaseName << std::endl;
        
        INIFile file;
        if (!file.Read(remapsDir + "/" + remapBaseName + "/config.ini"))
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not read config.ini" << std::endl;
            continue;
        }

        auto sections = file.GetSections("PED");
        if (sections.size() == 0) continue;
        
        auto section = sections[0];
        
        int modelId = section->GetIntWithDefaultValue("ped_model_id", 0);

        if (modelId == 0) continue;

        auto modelInfo = RemapModelInfos::AddModelInfo(modelId, eRemapType::REMAP_SKIN);

        auto remapNames = ModConfig::GetDirectoriesName(remapsDir + "/" + remapBaseName);
        for(auto remapName : remapNames)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Reading remap: " << remapName << std::endl;

            auto remap = modelInfo->TryCreateRemap(remapName);

            auto files = ModConfig::GetFilesName(remapsDir + "/" + remapBaseName + "/" + remapName);
            for(auto file : files)
            {
                Log::Level(LOG_LEVEL::LOG_BOTH) << "Found texture: " << file << std::endl;

                RemapTexure remapTexture;
                remapTexture.textureName = ModConfig::ReplaceExtension(file, "");
                remapTexture.textureFile = remapsDir + "/" + remapBaseName + "/" + remapName + "/" + file;
                remapTexture.texture = NULL;

                remap->remapTextures.push_back(remapTexture);
            }
        }
    }   
}

RemapModelInfo* RemapModelInfos::AddModelInfo(int modelId, eRemapType type)
{
    auto modelInfo = new RemapModelInfo(modelId, type);
    m_ModelInfos[modelId] = modelInfo;
    return modelInfo;
}

bool RemapModelInfos::HasModelInfo(int modelId)
{
    return m_ModelInfos.count(modelId);
}

RemapModelInfo* RemapModelInfos::GetModelInfo(int modelId)
{
    if(!HasModelInfo(modelId)) return NULL;

    return m_ModelInfos[modelId];
}