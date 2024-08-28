#include "Vehicle.h"

#include "CleoOpcodes.h"
#include "Log.h"
#include "Mod.h"
#include "VehicleDummy.h"
#include "RemapModelInfos.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern void* (*GetVehicleFromRef)(int);
extern CVehicle* (*FindPlayerVehicle)(int playerId, bool bIncludeRemote);

extern RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
extern RpMaterial* (*RpMaterialSetTexture)(RpMaterial* material, RwTexture* texture);

static std::list<std::pair<unsigned int *, unsigned int>> resetEntries;
static std::list<std::pair<RpMaterial*, RwTexture*>> resetMaterials;

static RwTexture* testTexture;

Vehicle::Vehicle(int hVehicle)
{
    this->hVehicle = hVehicle;

    this->pVehicle = (CVehicle*)GetVehicleFromRef(hVehicle);

    this->modelId = GET_CAR_MODEL(hVehicle);

    SetRandomRemap();
}

Vehicle::~Vehicle()
{
    
}

void Vehicle::Update(int dt)
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: update" << std::endl;

    //CVehicle* playerVehicle = FindPlayerVehicle(0, false);
}

void Vehicle::RenderBefore()
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderBefore " << hVehicle << std::endl;

    //CVehicle* playerVehicle = FindPlayerVehicle(0, false);

    if(!remap) return;

    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "setting remap" << std::endl;

    auto atomics = VehicleDummy::RpClumpGetAllAtomics(pVehicle->m_pRwClump);
    for(auto atomic : atomics)
    {
        if(!atomic->geometry) continue;

        atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;

        auto materials = VehicleDummy::RpGeometryGetAllMaterials(atomic->geometry);
        for(auto material : materials)
        {
            if(!material->texture) continue;

            std::string name = std::string(material->texture->name);

            for(int i = 0; i < remap->remapTextures.size(); i++)
            {
                auto remapTexture = &remap->remapTextures[i];

                if(name.find(remapTexture->textureName) == std::string::npos) continue;

                if(!remapTexture->texture)
                {
                    menuVSL->debug->AddLine("Loading texture: " + remapTexture->textureName + " from remap " + remap->name);

                    remapTexture->texture = (RwTexture*)menuVSL->LoadRwTextureFromFile(remapTexture->textureFile, remapTexture->textureName);
                }
                
                RpMaterialSetTexture(material, remapTexture->texture);

                if(material->texture != remapTexture->texture)
                {
                    //resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int *>(&material->texture), *reinterpret_cast<unsigned int *>(&material->texture)));

                    //resetMaterials.push_back(std::make_pair(material, material->texture));

                    material->color = {255, 255, 255, 255};
                    

                    //menuVSL->debug->AddLine("Updated texture: " + name + " to " + remapTexture->textureName);
                }
            }
        }
    }
}

void Vehicle::RenderAfter()
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderAfter " << hVehicle << std::endl;

    for (auto &p : resetEntries)
    *p.first = p.second;
    resetEntries.clear();

    for(auto p : resetMaterials) RpMaterialSetTexture(p.first, p.second);
    resetMaterials.clear();
}

void Vehicle::SetRemap(std::string name)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: SetRemap " << name << std::endl;

    auto modelInfo = RemapModelInfos::GetModelInfo(this->modelId);

    if(!modelInfo)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Model info " << this->modelId << " not found" << std::endl;
        return;
    }

    if(name == "random")
    {
        SetRandomRemap();
        return;
    }

    if(!modelInfo->HasRemap(name))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Model info does not have remap " << name << std::endl;
        return;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Set remap to: " << name << std::endl;

    remap = modelInfo->GetRemap(name);
}

void Vehicle::SetRandomRemap()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: SetRandomRemap" << std::endl;

    auto modelInfo = RemapModelInfos::GetModelInfo(this->modelId);

    if(!modelInfo) return;

    remap = modelInfo->GetRandomRemap();
}