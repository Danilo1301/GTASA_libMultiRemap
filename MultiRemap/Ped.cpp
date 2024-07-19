#include "Ped.h"

#include "CleoFunctions.h"
#include "Log.h"
#include "Mod.h"
#include "VehicleDummy.h"
#include "RemapModelInfos.h"
#include "Vehicles.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern void* (*GetPedFromRef)(int);
//extern CVehicle* (*FindPlayerVehicle)(int playerId, bool bIncludeRemote);

extern RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
extern RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
extern RpMaterial* (*RpMaterialSetTexture)(RpMaterial* material, RwTexture* texture);

static std::list<std::pair<unsigned int *, unsigned int>> resetEntries;
static std::list<std::pair<RpMaterial*, RwTexture*>> resetMaterials;

static RwTexture* testTexture;

Ped::Ped(int hPed)
{
    this->hPed = hPed;

    this->pPed = (CPed*)GetPedFromRef(hPed);

    UpdateSkin();
    SetRandomRemap();
}

Ped::~Ped()
{
    
}

void Ped::Update(int dt)
{
    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped: update" << std::endl;

    UpdateSkin();
    UpdateVehicleRemap();
}

void Ped::UpdateVehicleRemap()
{
    auto modelInfo = RemapModelInfos::GetModelInfo(this->modelId);

    if(!modelInfo) return;

    if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(hPed)) return;

    auto vehicleHandle = CleoFunctions::ACTOR_USED_CAR(hPed);

    auto vehicle = Vehicles::GetVehicleByHandle(vehicleHandle);

    if(!vehicle) return;

    if(!vehicle->remap) return;

    auto remapName = vehicle->remap->name;
    
    SetRemap(remapName);
}

void Ped::UpdateSkin()
{
    auto prevModelId = this->modelId;   
    this->modelId = CleoFunctions::GET_CHAR_MODEL(hPed);

    if(prevModelId != modelId)
    {
        SetRandomRemap();
    }
}

void Ped::RenderBefore()
{
    if(!remap) return;

    auto atomics = VehicleDummy::RpClumpGetAllAtomics(pPed->m_pRwClump);
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
                
                //resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int *>(&material->color), *reinterpret_cast<unsigned int *>(&material->color)));
                //material->color = {255, 255, 255, 255};

                //resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int *>(&material->texture), *reinterpret_cast<unsigned int *>(&material->texture)));
                RpMaterialSetTexture(material, remapTexture->texture);
                //material->texture = 0;


                if(material->texture != remapTexture->texture)
                {
                    

                    //resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int *>(&material->texture), *reinterpret_cast<unsigned int *>(&material->texture)));
                    //resetMaterials.push_back(std::make_pair(material, material->texture));

                    //menuVSL->debug->m_Visible = true;
                    //menuVSL->debug->AddLine("Updated texture: " + name + " to " + remapTexture->textureName);
                }
            }
        }
    }
}

void Ped::RenderAfter()
{
    for (auto &p : resetEntries)
    *p.first = p.second;
    resetEntries.clear();

    for(auto p : resetMaterials) RpMaterialSetTexture(p.first, p.second);
    resetMaterials.clear();
}

void Ped::SetRemap(std::string name)
{
    if(remap)
    {
        if(remap->name == name) return;
    }

    auto modelInfo = RemapModelInfos::GetModelInfo(this->modelId);

    if(!modelInfo) return;

    if(!modelInfo->HasRemap(name)) return;

    remap = modelInfo->GetRemap(name);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Set remap to: " << remap->name << std::endl;
}

void Ped::SetRandomRemap()
{
    auto modelInfo = RemapModelInfos::GetModelInfo(this->modelId);

    if(!modelInfo) return;

    remap = modelInfo->GetRandomRemap();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Random remap selected: " << remap->name << std::endl;
}