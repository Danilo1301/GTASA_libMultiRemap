#include "pch.h"

#include "RemapModelInfo.h"
            
class RemapModelInfos {
public:
    static std::map<int, RemapModelInfo*> m_ModelInfos;

    static void Load();

    static RemapModelInfo* AddModelInfo(int modelId);
    static bool HasModelInfo(int modelId);
    static RemapModelInfo* GetModelInfo(int modelId);
};