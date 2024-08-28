LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := MultiRemap
LOCAL_CPP_FEATURES += exceptions
LOCAL_SRC_FILES := main.cpp mod/logger.cpp mod/config.cpp Log.cpp json_reader.cpp json_value.cpp json_writer.cpp iniconfig/INIFile.cpp iniconfig/INISection.cpp sdk/Image.cpp Mod.cpp Widgets.cpp windows/WindowMain.cpp Vehicle.cpp Vehicles.cpp VehicleDummy.cpp ModConfig.cpp RemapModelInfos.cpp RemapModelInfo.cpp Ped.cpp Peds.cpp MultiRemap.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
LOCAL_C_INCLUDES += ./include
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)