#include <stdint.h>
#include <string>

#include "cleo.h"

#include <mod/logger.h>

#define __decl_op(__name, __int)	const char* NAME_##__name = #__name; const uint16_t OP_##__name = __int;

#define __handler_params 			void *handle, uint32_t *ip, uint16_t opcode, const char *name

// Size of array
#define sizeofA(__aVar)  ((int)(sizeof(__aVar)/sizeof(__aVar[0])))

#define __reg_opcode				cleo->RegisterOpcode

#define __reg_func					cleo->RegisterOpcodeFunction

#define __op_name_match(x) 			opcode == OP_##x || strcmp(name, NAME_##x) == 0

#define __reg_op_func(x, h) 		__reg_opcode(OP_##x, h); __reg_func(NAME_##x, h);

#define __readParam(handle)         reinterpret_cast<cleo_ifs_t::data_t*>(cleo->ReadParam(handle))
#define __getPointerToScriptVar(handle)         reinterpret_cast<cleo_ifs_t::data_t*>(cleo->GetPointerToScriptVar(handle))

#include "Log.h"
#include "RemapModelInfo.h"
#include "Mod.h"

#include "windows/WindowMain.h"

extern cleo_ifs_t* cleo;

__decl_op(MULTI_REMAP_TOGGLE_MENU, 0x0EE0); // 0EE0=1,multi_remap_toggle_menu type %1d%

static void MULTI_REMAP_TOGGLE_MENU(__handler_params)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "MULTI_REMAP_TOGGLE_MENU" << std::endl;

    int type = __readParam(handle)->i;

    WindowMain::ToggleMenu((eRemapType)type);
}