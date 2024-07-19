#pragma once

#include "menu/IMenuVSL.h"

#include "Vehicle.h"
#include "Ped.h"
#include "RemapModelInfo.h"

class WindowMain {
public:
	static IWindow* m_Window;
	static Vehicle* m_Vehicle;
	static Ped* m_Ped;

	static void ToggleMenu(eRemapType type);
	static void CreateVehicle(Vehicle* vehicle);
	static void CreatePed(Ped* ped);
	static void Remove();
};