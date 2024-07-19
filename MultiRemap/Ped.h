#pragma once

#include "pch.h"

#include "SimpleGTA.h"

#include "RemapModelInfo.h"

class Ped {
public:
    int hPed;
    CPed* pPed = NULL;
    int modelId = -1;

    Remap* remap = NULL;

    Ped(int hPed);
	~Ped();

    void Update(int dt);
    void UpdateVehicleRemap();
    void UpdateSkin();
    
    void RenderBefore();
    void RenderAfter();

    void SetRemap(std::string name);
    void SetRandomRemap();
};