#include "MultiRemap.h"

#include "Log.h"
#include "RemapModelInfos.h"
#include "Vehicles.h"

std::vector<std::string> MultiRemap::GetModelInfoRemaps(int modelId)
{
    auto modelInfo = RemapModelInfos::GetModelInfo(modelId);

    if(!modelInfo)
    {
        return {};
    }

    std::vector<std::string> remaps;

    for(auto p : modelInfo->remaps)
    {
        auto remap = p.second;
        
        remaps.push_back(remap->name);
    }

    return remaps;
}

void MultiRemap::SetVehicleRemap(int hVehicle, std::string remap)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Interface: SetVehicleRemap " << hVehicle << " to " << remap << std::endl;

    auto vehicle = Vehicles::GetVehicleByHandle(hVehicle);

    if(!vehicle) vehicle = Vehicles::TryCreateVehicle(hVehicle);

    if(!vehicle)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle not found" << std::endl;
        return;
    }

    vehicle->SetRemap(remap);
}

std::string MultiRemap::GetVehicleRemap(int hVehicle)
{
    auto vehicle = Vehicles::GetVehicleByHandle(hVehicle);

    if(!vehicle) vehicle = Vehicles::TryCreateVehicle(hVehicle);
    
    if(!vehicle) return "";

    if(!vehicle->remap) return "";

    return vehicle->remap->name;
}

static MultiRemap multiRemap_local;
IMultiRemap* multiRemap = &multiRemap_local;