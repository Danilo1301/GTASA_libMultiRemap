#pragma once

#include <vector>
#include <string>

class IMultiRemap {
public:

    /*1.1.1*/
    
    virtual std::vector<std::string> GetModelInfoRemaps(int modelId) = 0;

    /*
    * Use remap 'random' to randomize
    */
    virtual void SetVehicleRemap(int hVehicle, std::string remap) = 0;

    virtual std::string GetVehicleRemap(int hVehicle) = 0;
};