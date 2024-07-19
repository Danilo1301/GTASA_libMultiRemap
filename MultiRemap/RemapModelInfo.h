#pragma once

#include "pch.h"

enum eRemapType {
    REMAP_VEHICLE,
    REMAP_SKIN
};

/*
configs/multiPlotagem/remaps (list of remaps)
    pmesp (Remap)
        vidro1.png (RemapTexture)
        thiago.png (RemapTexture)
*/

struct RemapTexure
{
    std::string textureName = "";
    std::string textureFile = "";
    RwTexture* texture = NULL;
};

class Remap {
public:
    std::string name;
    std::vector<RemapTexure> remapTextures;
};

class RemapModelInfo {
public:
    int modelId;
    eRemapType type;
    std::map<std::string, Remap*> remaps;

    RemapModelInfo(int modelId, eRemapType type);

    bool HasRemap(std::string name);
    Remap* TryCreateRemap(std::string name);
    Remap* GetRemap(std::string name);
    Remap* GetRandomRemap();
};