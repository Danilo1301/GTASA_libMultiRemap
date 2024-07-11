#pragma once

#include "pch.h"

#include "Vehicle.h"

class Vehicles {
public:
    static std::map<int, Vehicle*> m_Vehicles;

    static void Update(int dt);

    static void TryFindNewVehicles();

    static bool HasVehicleHandle(int hVehicle);
    static Vehicle* TryCreateVehicle(int hVehicle);
    static Vehicle* GetVehicleByHandle(int hVehicle);
    static Vehicle* GetVehicleByCVehicle(CVehicle* pVehicle);
    static void RemoveVehicle(int hVehicle);
};