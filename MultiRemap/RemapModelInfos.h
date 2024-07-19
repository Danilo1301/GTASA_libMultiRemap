#include "pch.h"

#include "RemapModelInfo.h"
            
class RemapModelInfos {
public:
    static std::map<int, RemapModelInfo*> m_ModelInfos;

    static void Load();
    static void LoadVehicles();
    static void LoadSkins();

    static RemapModelInfo* AddModelInfo(int modelId, eRemapType type);
    static bool HasModelInfo(int modelId);
    static RemapModelInfo* GetModelInfo(int modelId);
};