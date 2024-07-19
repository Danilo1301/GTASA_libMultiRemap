#include "ModConfig.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
//#include <dirent.h>
//#include "dlfcn.h"

#include "mod/amlmod.h"
#include "mod/logger.h"
#include "mod/config.h"

#include "iniconfig/INIFile.h"

#include "Mod.h"
#include "Log.h"
#include "RemapModelInfos.h"

bool isDirExist(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

bool file_exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::vector<std::string> get_directories_name(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::directory_iterator(s))
        if (p.is_directory())
            r.push_back(p.path().filename().string());
    return r;
}

std::vector<std::string> get_files_name(const std::string& s)
{
    std::vector<std::string> r;
    for (auto& p : std::filesystem::directory_iterator(s))
        if (!p.is_directory())
            r.push_back(p.path().filename().string());
    return r;
}

std::string replace_extension(const std::string& filePath, const std::string& newExtension) {
    // Create a path object from the input file path
    std::filesystem::path pathObj(filePath);

    // Replace the extension
    pathObj.replace_extension(newExtension);

    // Return the string representation of the modified path
    return pathObj.string();
}

//

std::vector<VersionInfo*> VersionControl::m_Versions;
std::string VersionControl::m_PrevVersion = "";
std::string VersionControl::m_CurrentVersion = "";

void VersionControl::SetVersion(std::string prevVersion, std::string currentVersion)
{
    m_PrevVersion = prevVersion;
    m_CurrentVersion = currentVersion;
}

void VersionControl::AddVersion(std::string version)
{
    VersionInfo* info = new VersionInfo();
    info->version = version;

    m_Versions.push_back(info);
}

VersionInfo* VersionControl::GetVersionInfo(std::string version)
{
    for(auto info : m_Versions)
    {
        if(info->version == version) return info;
    }
    return NULL;
}

void VersionControl::AddPatch(std::string version, std::function<void()> patch)
{
    VersionInfo* info = GetVersionInfo(version);
    info->patches.push_back(patch);
}

void VersionControl::AddPatch(std::function<void()> patch)
{
    for(auto info : m_Versions)
    {
        info->patches.push_back(patch);
    }
}

void VersionControl::ApplyPatches()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: ApplyPatches" << std::endl;

    auto prevVersion = m_PrevVersion;
    VersionInfo* prevInfo = NULL;

    if(prevVersion == m_CurrentVersion)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Same version, no need to apply patches" << std::endl;
        return;
    }

    if(prevVersion == "unknown")
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Version is unknown, so its the first time run, no need to apply patches" << std::endl;
        return;
    }

    int index = 0;
    for(auto info : m_Versions)
    {
        if(info->version == prevVersion)
        {
            prevInfo = info;
            break;
        }

        index++;
    }

    while (index < m_Versions.size() - 1)
    {
        prevInfo = m_Versions[index];

        Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Processing index " << index << ", version " << prevInfo->version << std::endl;

        if(prevInfo->patches.size() > 0)
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "VersionControl: Applying " << prevInfo->patches.size() << " patches..." << std::endl;
        }

        for(auto patch : prevInfo->patches)
        {
            patch();
        }
        prevInfo->patches.clear();

        index++;
    }
}

//

std::string ModConfig::m_ConfigMainFolderName = "multiRemap";

void ModConfig::MakePaths()
{
    CreateFolder(GetConfigFolder());
}

bool ModConfig::DirExists(std::string path)
{
    return isDirExist(path);
}

bool ModConfig::FileExists(std::string path)
{
    return file_exists(path);
}

std::vector<std::string> ModConfig::GetDirectoriesName(std::string path)
{
    return get_directories_name(path);
}

std::vector<std::string> ModConfig::GetFilesName(std::string path)
{
    return get_files_name(path);
}

std::string ModConfig::ReplaceExtension(std::string path, std::string newExtension)
{
    return replace_extension(path, newExtension);
}

void ModConfig::ConfigDelete(std::string path)
{
    try {
        if (std::filesystem::remove(path))
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: file " << path << " deleted" << std::endl;
        else
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: file " << path << " not found" << std::endl;
    }
    catch(const std::filesystem::filesystem_error& err) {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: delete file: filesystem error: " << err.what() << std::endl;
    }
}

std::string ModConfig::GetConfigFolder()
{
    char path[0xFF];
    snprintf(path, sizeof(path), "%s/%s", aml->GetConfigPath(), m_ConfigMainFolderName.c_str());
    return path;
}

void ModConfig::CreateFolder(std::string path)
{
    if (DirExists(path)) return;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: CreateFolder " << path << std::endl;

    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void ModConfig::WriteToFile(std::string path, Json::Value value) {
	Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: WriteToFile " << path << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(path);
	file << strJson;
	file.close();
}

Json::Value ModConfig::ReadFile(std::string path) {
	Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: ReadFile " << path << std::endl;

	std::ifstream file(path);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		//MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), "GiroflexVSL", MB_ICONERROR);
	}

	return value;
}

void ModConfig::Save()
{
    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Save" << std::endl;

    MakePaths();
}

void ModConfig::Load()
{
    MakePaths();

    RemapModelInfos::Load();
}

std::string ModConfig::ReadVersionFile()
{
    std::string prevVersion = "unknown";
    std::string path = GetConfigFolder() + "/version";

    std::ifstream file;
    file.open(path);
    if (file.good())
    {
        getline(file, prevVersion);
    }
    file.close();

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: ReadVersionFile, version: " << prevVersion << std::endl;

    return prevVersion;
}

void ModConfig::DefineVersions()
{
    VersionControl::AddVersion("1.0.0");

    VersionControl::SetVersion(ReadVersionFile(), Mod::m_Version);
}

void ModConfig::ProcessVersionChanges_PreConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [PRE] Updating from " << prevVersion << " to " << currentVersion << std::endl;

    VersionControl::ApplyPatches();
}

void ModConfig::ProcessVersionChanges_PostConfigLoad()
{
    std::string prevVersion = ReadVersionFile();
    std::string currentVersion = Mod::m_Version;

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: [POST] Updating from " << prevVersion << " to " << currentVersion << std::endl;

    VersionControl::ApplyPatches();

    //

    Log::Level(LOG_LEVEL::LOG_BOTH) << "ModConfig: Saving version file" << std::endl;

    std::string path = GetConfigFolder() + "/version";

    std::fstream file;
    file.open(path, std::fstream::out);
    file << currentVersion;
    file.close();
}