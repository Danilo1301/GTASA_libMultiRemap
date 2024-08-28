#include "Vehicles.h"

#include "SimpleGTA.h"

#include "Log.h"
#include "Mod.h"

extern uintptr_t* pVehiclePool;
extern int (*GetVehicleRef)(int);

std::map<int, Vehicle*> Vehicles::m_Vehicles;

void Vehicles::Update(int dt)
{
    TryFindNewVehicles();

    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;

        vehicle->Update(dt);
    }
}

void Vehicles::TryFindNewVehicles()
{   
    std::vector<Vehicle*> vehiclesThatExists;

    //GET_RANDOM_CAR_IN_SPHERE_NO_SAVE_RECURSIVE
    //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp

    auto objects = *(GTAVehicleSA**)(*pVehiclePool + 0);
    tByteFlag* flags = *(tByteFlag**)(*pVehiclePool + 4);
    int size = *(int*)(*pVehiclePool + 8);

    for (int i = 0; i < size; ++i)
    {
        if (flags[i].bEmpty) continue;
        auto& ent = objects[i];
        
        //IS_CAR_SIREN_ON
        //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp
        // 
        //bool sirenOn = *(uint8_t*)((uintptr_t)vehicle + 0x42D + 4) >> 7;
        bool sirenOn = ent.UInt8At(0x42D + 4) >> 7;

        void* driver = (void*)ent.UIntAt(0x460 + 4);

        int ref = GetVehicleRef(ent.AsInt());

        Vehicle* vehicle = NULL;

        if(!HasVehicleHandle(ref))
        {
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "Found " << ent.AsInt() << " ref " << ref << " siren " << (sirenOn ? "ON" : "OFF") << std::endl;
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "driver " << driver << std::endl;
            TryCreateVehicle(ref);
        }

        vehicle = GetVehicleByHandle(ref);

        vehiclesThatExists.push_back(vehicle);
    }

    std::vector<int> vehiclesToDelete;

    for(auto p : m_Vehicles)
    {
        auto vehicle = p.second;

        auto it = std::find(vehiclesThatExists.begin(), vehiclesThatExists.end(), vehicle);

        if (it == vehiclesThatExists.end())
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not find vehicle " << vehicle << std::endl;
            vehiclesToDelete.push_back(vehicle->hVehicle);
        }
    }

    for(auto hVehicle : vehiclesToDelete)
    {
        RemoveVehicle(hVehicle);
    }
}

bool Vehicles::HasVehicleHandle(int hVehicle)
{
    return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::TryCreateVehicle(int hVehicle)
{   
    if(hVehicle == 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Bro, i think your code has some issues (ERROR) (Vehicles)" << std::endl;
        return NULL;
    }

	if (HasVehicleHandle(hVehicle))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle already added: " << hVehicle << std::endl;
        return GetVehicleByHandle(hVehicle);
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Add vehicle " << hVehicle << " (" << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;

    auto vehicle = new Vehicle(hVehicle);

	m_Vehicles[hVehicle] = vehicle;

    return vehicle;
}

Vehicle* Vehicles::GetVehicleByHandle(int hVehicle)
{
	if (!HasVehicleHandle(hVehicle)) return NULL;
	return m_Vehicles.at(hVehicle);
}

Vehicle* Vehicles::GetVehicleByCVehicle(CVehicle* pVehicle)
{
    for(auto pair : m_Vehicles)
    {
        auto vehicle = pair.second;
        if(vehicle->pVehicle == pVehicle) return vehicle;
    }
    return NULL;
}

void Vehicles::RemoveVehicle(int hVehicle)
{
    auto vehicle = m_Vehicles[hVehicle];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Delete vehicle " << hVehicle << ", ref: " << vehicle->pVehicle << ", model: " << vehicle->modelId << " (" << std::to_string(m_Vehicles.size() - 1) << " total)" << std::endl;

    m_Vehicles.erase(hVehicle);
    delete vehicle;
}