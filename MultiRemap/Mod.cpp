#include "Mod.h"

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "Log.h"
#include "CleoOpcodes.h"
#include "Widgets.h"
#include "Vehicles.h"
#include "Peds.h"

#include "windows/WindowMain.h"

#include "menu/IMenuVSL.h"
extern IMenuVSL* menuVSL;

extern RwRaster* (*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
extern RwRaster* (*RwRasterUnlock)(RwRaster* raster);
extern RwBool (*RwTextureSetMipmapping)(RwBool enable);
extern RwBool (*RwTextureSetAutoMipmapping)(RwBool enable);
extern RwTexture* (*RwTextureCreate)(RwRaster* raster);
extern RwUInt8* (*RwRasterLock)(RwRaster* raster, RwUInt8 level, RwInt32 lockMode);
extern RpMaterial* (*RpMaterialSetTexture)(RpMaterial* material, RwTexture* texture);

//---------------------

extern void* (*FindPlayerPed)(int playerId);
extern CVehicle* (*FindPlayerVehicle)(int playerId, bool bIncludeRemote);

std::string Mod::m_Version = "1.2.0";

void Mod::Update(int dt)
{
    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "Mod: Update " << dt << std::endl;

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "input" << std::endl;

    Widgets::Update(dt);

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "vehicles" << std::endl;

    Vehicles::Update(dt);

    //Log::Level(LOG_LEVEL::LOG_UPDATE) << "peds" << std::endl;

    Peds::Update(dt);
}

void Mod::Init()
{
    
}

void Mod::ToggleMainMenu()
{
    
}

std::string Mod::GetConfigPath(std::string path)
{
    char buffer[512];
    sprintf(buffer, "%s/%s", aml->GetConfigPath(), path.c_str());
    return buffer;
}