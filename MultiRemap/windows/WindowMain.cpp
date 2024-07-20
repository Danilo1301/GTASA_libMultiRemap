#include "WindowMain.h"

#include "RemapModelInfos.h"
#include "Log.h"
#include "Vehicles.h"
#include "Peds.h"

#include "MultiRemap.h"

extern IMenuVSL* menuVSL;

extern CVehicle* (*FindPlayerVehicle)(int playerId, bool bIncludeRemote);
extern void* (*FindPlayerPed)(int playerId);

IWindow* WindowMain::m_Window = NULL;
Vehicle* WindowMain::m_Vehicle = NULL;
Ped* WindowMain::m_Ped = NULL;

void WindowMain::ToggleMenu(eRemapType type)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowMain: ToggleMenu" << std::endl;

    if(m_Window)
    {
        Remove();
        return;
    }

    if(type == eRemapType::REMAP_VEHICLE)
    {
        auto playerVehicle = FindPlayerVehicle(0, false);

        if(!playerVehicle) return;

        auto vehicle = Vehicles::GetVehicleByCVehicle(playerVehicle);

        WindowMain::CreateVehicle(vehicle);
    }

    if(type == eRemapType::REMAP_SKIN)
    {
        CPed* playerPed = (CPed*)FindPlayerPed(0);

        if(!playerPed) return;

        auto ped = Peds::GetPedByCPed(playerPed);

        WindowMain::CreatePed(ped);
    }
}

void WindowMain::CreateVehicle(Vehicle* vehicle)
{
	if (m_Window) return;

    m_Vehicle = vehicle;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowMain: Create for ID " << vehicle->modelId << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: " << vehicle << " | " << vehicle->pVehicle << std::endl;

    auto modelInfo = RemapModelInfos::GetModelInfo(vehicle->modelId);
    
    auto window = m_Window = menuVSL->AddWindow();
    //window->m_Position = CVector2D(200, 200);
    window->m_Title = "Multi Remap";

    auto test1 = window->AddButton("Set remap to pmesp", CRGBA(255, 255, 255));
    test1->onClick = [vehicle]() {
        multiRemap->SetVehicleRemap(vehicle->hVehicle, "pmesp");

        auto remaps = multiRemap->GetModelInfoRemaps(vehicle->modelId);
        auto currentRemap = multiRemap->GetVehicleRemap(vehicle->hVehicle);

        menuVSL->debug->m_Visible = true;
        menuVSL->debug->AddLine(std::to_string(remaps.size()) + " remaps");
        menuVSL->debug->AddLine("currentRemap: " + currentRemap);
    };

    window->AddText("Vehicle ID ~g~" + std::to_string(vehicle->modelId), CRGBA(255, 255, 255));

    if(modelInfo)
    {
        window->AddText("Selecione um remap:", CRGBA(255, 255, 255));

        for(auto p : modelInfo->remaps)
        {
            auto remap = p.second;

            auto button = window->AddButton("> ~y~" + remap->name, CRGBA(0, 0, 0, 0));
            button->onClick = [vehicle, remap]() {
                vehicle->SetRemap(remap->name);
            };
        }
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowMain: The car ID " << vehicle->modelId << " has no remaps" << std::endl;

        window->AddText("Este carro nao tem nenhum remap", CRGBA(255, 255, 255));
    }

    auto close = window->AddButton("~r~Close", CRGBA(255, 255, 255));
    close->onClick = []() {
        Remove();
    };
}

void WindowMain::CreatePed(Ped* ped)
{
	if (m_Window) return;

    m_Ped = ped;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowMain: Create for ID " << ped->modelId << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: " << ped << " | " << ped->pPed << std::endl;

    auto modelInfo = RemapModelInfos::GetModelInfo(ped->modelId);
    
    auto window = m_Window = menuVSL->AddWindow();
    //window->m_Position = CVector2D(200, 200);
    window->m_Title = "Multi Remap";

    window->AddText("Skin ID ~g~" + std::to_string(ped->modelId), CRGBA(255, 255, 255));

    if(modelInfo)
    {
        window->AddText("Selecione um remap:", CRGBA(255, 255, 255));

        for(auto p : modelInfo->remaps)
        {
            auto remap = p.second;

            auto button = window->AddButton("> ~y~" + remap->name, CRGBA(0, 0, 0, 0));
            button->onClick = [ped, remap]() {
                ped->SetRemap(remap->name);
            };
        }
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowMain: The ped ID " << ped->modelId << " has no remaps" << std::endl;

        window->AddText("Esta skin nao tem nenhum remap", CRGBA(255, 255, 255));
    }

    auto close = window->AddButton("~r~Close", CRGBA(255, 255, 255));
    close->onClick = []() {
        Remove();
    };
}

void WindowMain::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
    m_Vehicle = NULL;
}