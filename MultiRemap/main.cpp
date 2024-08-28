#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

MYMODCFG(net.danilo1301.multiRemap, Multi Remap, 1.1.1, Danilo1301)

//MYMOD(net.rusjj.mymod.guid, AML Mod Template Without Config, 1.0, RusJJ)

//NEEDGAME(net.rusjj.mygame)

//BEGIN_DEPLIST()
//    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.0)
//END_DEPLIST()

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// Menu VSL
#include "menu/IMenuVSL.h"
IMenuVSL* menuVSL = NULL;

// Script Commands
#include "IScriptCommands.h"
IScriptCommands* scriptCommands = NULL;

#include "MultiRemap.h"

#include "SimpleGTA.h"

#include "Log.h"
#include "Mod.h"
#include "Vehicles.h"
#include "Peds.h"
#include "ModConfig.h"
#include "utils.h"

#include "opcodes.h"

#include "VehicleDummy.h"

//

uintptr_t* pVehiclePool = 0;
int (*GetVehicleRef)(int);
void* (*GetVehicleFromRef)(int);
uintptr_t* pPedPool = 0;
int (*GetPedRef)(int);
void* (*GetPedFromRef)(int);

RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
RwRaster* (*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwRaster* (*RwRasterUnlock)(RwRaster* raster);
RwBool (*RwTextureSetMipmapping)(RwBool enable);
RwBool (*RwTextureSetAutoMipmapping)(RwBool enable);
RwTexture* (*RwTextureCreate)(RwRaster* raster);
RwUInt8* (*RwRasterLock)(RwRaster* raster, RwUInt8 level, RwInt32 lockMode);
RpMaterial* (*RpMaterialSetTexture)(RpMaterial* material, RwTexture* texture);
char* (*GetFrameNodeName)(RwFrame* frame);

void* (*FindPlayerPed)(int playerId);
CVehicle* (*FindPlayerVehicle)(int playerId, bool bIncludeRemote);

//

static RwTexture* testTexture;

/*
DECL_HOOKv(RenderVehicle, void* self)
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderVehicle" << std::endl;
    RenderVehicle(self);
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderVehicle end" << std::endl;
}
*/

DECL_HOOKv(RenderVehicle, void* self)
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderVehicle" << std::endl;

    CVehicle* pVehicle = (CVehicle*)self;
    //CVehicle* playerVehicle = FindPlayerVehicle(0, false);

    auto vehicle = Vehicles::GetVehicleByCVehicle(pVehicle);

    if(vehicle) vehicle->RenderBefore();
    
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "calling fn" << std::endl;
    RenderVehicle(self);
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "end calling fn" << std::endl;
    
    if(vehicle) vehicle->RenderAfter();

    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderVehicle end" << std::endl;
}

DECL_HOOKv(RenderPed, void* self)
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderPed" << std::endl;

    CPed* pPed = (CPed*)self;

    /*
    if(!testTexture)
    {
        auto path = ModConfig::GetConfigFolder() + "/test.png";
        testTexture = (RwTexture*)menuVSL->LoadRwTextureFromFile(path, "test");
    }
    */

    auto ped = Peds::GetPedByCPed(pPed);

    if(ped) ped->RenderBefore();
    
    RenderPed(self);
    
    if(ped) ped->RenderAfter();

    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "RenderPed end" << std::endl;
}

DECL_HOOK(void*, UpdateGameLogic, uintptr_t a1)
{
    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "UpdateGameLogic" << std::endl;

    Mod::Update(menuVSL->deltaTime);

    Log::Level(LOG_LEVEL::LOG_DEEP_UPDATE) << "UpdateGameLogic end" << std::endl;

    return UpdateGameLogic(a1);
}

extern "C" void OnModPreLoad()
{
    ModConfig::MakePaths();

    char logPath[512];
    sprintf(logPath, "%s/multiRemap/", aml->GetConfigPath());
    Log::Open(logPath, "multiRemap");
    Log::deepLogEnabled = true;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload()" << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering interface 'MultiRemap'..." << std::endl;

    RegisterInterface("MultiRemap", multiRemap);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Preload() END" << std::endl;
}

extern "C" void OnModLoad()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load()" << std::endl;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading interfaces..." << std::endl;

    LoadInterface(&cleo, "CLEO");
    LoadInterface(&menuVSL, "MenuVSL");
    LoadInterface(&scriptCommands, "ScriptCommands");

    if(!menuVSL)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL is not installled" << std::endl;
        return;
    }

    if(aml->HasModOfBiggerVersion("net.danilo1301.menuvsl", "1.1.0"))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "MenuVSL version should be correct!" << std::endl;
    } else {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "You need to update MenuVSL!" << std::endl;
        return;
    }

    if(!scriptCommands)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ScriptCommands is not installled" << std::endl;
        return;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Finding symbols..." << std::endl;

    void* pGameHandle = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)(cleo->GetMainLibraryLoadAddress());

    SET_TO(pVehiclePool, aml->GetSym(pGameHandle, "_ZN6CPools15ms_pVehiclePoolE"));
    SET_TO(GetVehicleRef, aml->GetSym(pGameHandle, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(pGameHandle, "_ZN6CPools10GetVehicleEi"));

    SET_TO(pPedPool, aml->GetSym(pGameHandle, "_ZN6CPools11ms_pPedPoolE"));
    SET_TO(GetPedRef, aml->GetSym(pGameHandle, "_ZN6CPools9GetPedRefEP4CPed"));
    SET_TO(GetPedFromRef, aml->GetSym(pGameHandle, "_ZN6CPools6GetPedEi"));

    SET_TO(RpClumpForAllAtomics, aml->GetSym(pGameHandle, "_Z20RpClumpForAllAtomicsP7RpClumpPFP8RpAtomicS2_PvES3_"));
    SET_TO(RpGeometryForAllMaterials, aml->GetSym(pGameHandle, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));
    SET_TO(RwRasterCreate, aml->GetSym(pGameHandle, "_Z14RwRasterCreateiiii"));
    SET_TO(RwRasterUnlock, aml->GetSym(pGameHandle, "_Z14RwRasterUnlockP8RwRaster"));
    SET_TO(RwTextureSetMipmapping, aml->GetSym(pGameHandle, "_Z22RwTextureSetMipmappingi"));
    SET_TO(RwTextureSetAutoMipmapping, aml->GetSym(pGameHandle, "_Z26RwTextureSetAutoMipmappingi"));
    SET_TO(RwTextureCreate, aml->GetSym(pGameHandle, "_Z15RwTextureCreateP8RwRaster"));
    SET_TO(RwRasterLock, aml->GetSym(pGameHandle, "_Z12RwRasterLockP8RwRasterhi"));
    SET_TO(RpMaterialSetTexture, aml->GetSym(pGameHandle, "_Z20RpMaterialSetTextureP10RpMaterialP9RwTexture"));
    SET_TO(GetFrameNodeName, aml->GetSym(pGameHandle, "_Z16GetFrameNodeNameP7RwFrame"));
    
    SET_TO(FindPlayerPed, aml->GetSym(pGameHandle, "_Z13FindPlayerPedi"));
    SET_TO(FindPlayerVehicle, aml->GetSym(pGameHandle, "_Z17FindPlayerVehicleib"));

    HOOK(RenderVehicle, aml->GetSym(pGameHandle, "_ZN8CVehicle6RenderEv"));
    HOOK(RenderPed, aml->GetSym(pGameHandle, "_ZN4CPed6RenderEv"));
    HOOKPLT(UpdateGameLogic, gameAddr + 0x66FE58);

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Registering opcodes..." << std::endl;

    __reg_op_func(MULTI_REMAP_TOGGLE_MENU, MULTI_REMAP_TOGGLE_MENU);

    ModConfig::DefineVersions();
    ModConfig::ProcessVersionChanges_PreConfigLoad();
    ModConfig::Load();
    ModConfig::ProcessVersionChanges_PostConfigLoad();
    ModConfig::Save();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Load() END" << std::endl;
}
