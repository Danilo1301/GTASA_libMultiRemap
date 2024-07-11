#pragma once

#include "pch.h"

#include "sdk/Image.h"

class Mod {
public:
    static std::string m_Version;
    
    static void Update(int dt);
    static void Init();

    static void ToggleMainMenu();

    static std::string GetConfigPath(std::string path);
};