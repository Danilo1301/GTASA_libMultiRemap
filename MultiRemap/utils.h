#pragma once

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include <string>
#include <sys/stat.h>

#include "Log.h"

template <typename T>
static T* LoadInterface(T** out, std::string name)
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "Loading interface: " << name << "..." << std::endl;

    void* interface = GetInterface(name.c_str());

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Interface: " << interface << std::endl;

    *out = static_cast<T*>(interface);

    if (*out) Log::Level(LOG_LEVEL::LOG_BOTH) << name << " loaded" << std::endl;
    else Log::Level(LOG_LEVEL::LOG_BOTH) << name << " was not loaded" << std::endl;

    return *out;
}

static bool DirExists(std::string path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

static void CreateFolder(std::string path)
{
    if (DirExists(path))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Folder already exists: " << path << std::endl;
        return;
    }

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Create folder: " << path << std::endl;

    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}