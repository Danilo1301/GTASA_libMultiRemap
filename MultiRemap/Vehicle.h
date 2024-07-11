#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "RemapModelInfo.h"

class Vehicle {
public:
    int hVehicle;
    CVehicle* pVehicle = NULL;
    int modelId;

    Remap* remap = NULL;

    Vehicle(int hVehicle);
	~Vehicle();

    void Update(int dt);
    
    void RenderBefore();
    void RenderAfter();

    void SetRemap(std::string name);
    void SetRandomRemap();
};