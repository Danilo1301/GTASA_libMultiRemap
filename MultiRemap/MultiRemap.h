#pragma once

#include "IMultiRemap.h"

class MultiRemap : public IMultiRemap {
public:

    /*1.1.1*/

    std::vector<std::string> GetModelInfoRemaps(int modelId);

    void SetVehicleRemap(int hVehicle, std::string remap);

    std::string GetVehicleRemap(int hVehicle);
};

extern IMultiRemap* multiRemap;