#include "Peds.h"

#include "SimpleGTA.h"

#include "Log.h"
#include "CleoFunctions.h"
#include "Mod.h"

extern uintptr_t* pPedPool;
extern int (*GetPedRef)(int);

std::map<int, Ped*> Peds::m_Peds;

void Peds::Update(int dt)
{
    TryFindNewPeds();

    for(auto pair : m_Peds)
    {
        auto ped = pair.second;

        ped->Update(dt);
    }
}

void Peds::TryFindNewPeds()
{   
    std::vector<Ped*> pedsThatExist;

    //GET_RANDOM_CAR_IN_SPHERE_NO_SAVE_RECURSIVE
    //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp

    auto objects = *(GTAPedSA**)(*pPedPool + 0);
    tByteFlag* flags = *(tByteFlag**)(*pPedPool + 4);
    int size = *(int*)(*pPedPool + 8);

    for (int i = 0; i < size; ++i)
    {
        if (flags[i].bEmpty) continue;
        auto& ent = objects[i];
        
        auto ref = GetPedRef(ent.AsInt());

        Ped* ped = NULL;

        if(!HasPedHandle(ref))
        {
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "Found " << ent.AsInt() << " ref " << ref << " siren " << (sirenOn ? "ON" : "OFF") << std::endl;
            //Log::Level(LOG_LEVEL::LOG_BOTH) << "driver " << driver << std::endl;
            ped = TryCreatePed(ref);
        }

        ped = GetPedByHandle(ref);

        pedsThatExist.push_back(ped);
    }

    std::vector<int> pedsToDelete;

    for(auto p : m_Peds)
    {
        auto ped = p.second;

        auto it = std::find(pedsThatExist.begin(), pedsThatExist.end(), ped);

        if (it == pedsThatExist.end())
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not find ped " << ped << std::endl;
            pedsToDelete.push_back(ped->hPed);
        }
    }

    for(auto hPed : pedsToDelete)
    {
        RemovePed(hPed);
    }
}

bool Peds::HasPedHandle(int hPed)
{
    return m_Peds.find(hPed) != m_Peds.end();
}

Ped* Peds::TryCreatePed(int hPed)
{   
    if(hPed == 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Bro, i think your code has some issues (ERROR) (Peds)" << std::endl;
        return NULL;
    }

	if (HasPedHandle(hPed))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Ped already added: " << hPed << std::endl;
        return GetPedByHandle(hPed);
    }

    auto ped = new Ped(hPed);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Peds: Add ped " << hPed << ", ref: " << ped->pPed << ", model: " << ped->modelId << " (" << std::to_string(m_Peds.size() + 1) << " total)" << std::endl;

	m_Peds[hPed] = ped;

    return ped;
}

Ped* Peds::GetPedByHandle(int hPed)
{
	if (!HasPedHandle(hPed)) return NULL;
	return m_Peds.at(hPed);
}

Ped* Peds::GetPedByCPed(CPed* pPed)
{
    for(auto pair : m_Peds)
    {
        auto ped = pair.second;
        if(ped->pPed == pPed) return ped;
    }
    return NULL;
}

void Peds::RemovePed(int hPed)
{
    auto ped = m_Peds[hPed];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Peds: Delete ped " << hPed << ", ref: " << ped->pPed << ", model: " << ped->modelId << " (" << std::to_string(m_Peds.size() - 1) << " total)" << std::endl;

    m_Peds.erase(hPed);
    delete ped;
}