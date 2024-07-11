#pragma once

#include "menu/IMenuVSL.h"

#include "Vehicle.h"

class WindowMain {
public:
	static IWindow* m_Window;
	static Vehicle* m_Vehicle;

	static void Create(Vehicle* vehicle);
	static void Remove();
};