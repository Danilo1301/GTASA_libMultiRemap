#include "WindowMain.h"

#include "RemapModelInfos.h"
#include "Log.h"

IWindow* WindowMain::m_Window = NULL;
Vehicle* WindowMain::m_Vehicle = NULL;

extern IMenuVSL* menuVSL;

void WindowMain::Create(Vehicle* vehicle)
{
	if (m_Window) return;

    m_Vehicle = vehicle;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "WindowMain: Create for ID " << vehicle->modelId << std::endl;
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle: " << vehicle << " | " << vehicle->pVehicle << std::endl;

    auto modelInfo = RemapModelInfos::GetModelInfo(vehicle->modelId);
    
    auto window = m_Window = menuVSL->AddWindow();
    window->m_Title = "Multi Remap";

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

void WindowMain::Remove()
{
    m_Window->SetToBeRemoved();
    m_Window = NULL;
    m_Vehicle = NULL;
}