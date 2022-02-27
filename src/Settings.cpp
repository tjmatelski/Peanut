#include "Settings.h"
#include <Log.h>
#include <yaml-cpp/yaml.h>

#include <fstream>

namespace {
static std::filesystem::path settingsFile {};
static std::filesystem::file_time_type settingsLastUpdated {};
static YAML::Node node {};

void CheckForUpdateAndPopulateNode()
{
    if (settingsFile.empty()) {
        std::filesystem::path homeDir = std::getenv("HOME");
        if (homeDir.empty()) {
            LOG_ERROR("Couldn't find home directory!");
            return;
        }

        settingsFile = homeDir / ".config" / "Peanut" / "settings.yml";
        if (!std::filesystem::exists(settingsFile)) {
            LOG_INFO("Couldn't find settings file. Creating Settings file '{}'", settingsFile.string());
            std::filesystem::create_directories(settingsFile.parent_path());
            std::ofstream newSettingsFile(settingsFile);
            newSettingsFile << "ResourceDirectory: " << std::filesystem::current_path() << std::endl;
            newSettingsFile << "BuildDirectory: " << std::filesystem::current_path() << std::endl;
        }

        settingsLastUpdated = std::filesystem::last_write_time(settingsFile);
        node = YAML::LoadFile(settingsFile);
    }

    if (settingsLastUpdated != std::filesystem::last_write_time(settingsFile)) {
        node = YAML::LoadFile(settingsFile);
    }
}

}

namespace PEANUT {

void Settings::Initialize()
{
    CheckForUpdateAndPopulateNode();
}

std::filesystem::path Settings::GetResourceDir()
{
    CheckForUpdateAndPopulateNode();

    if (!std::filesystem::exists(settingsFile)) {
        LOG_ERROR("Settings file doesn't exist.");
        return {};
    }

    std::filesystem::path resourceDir(node["ResourceDirectory"].as<std::string>());
    if (!std::filesystem::exists(resourceDir)) {
        LOG_ERROR("Resource Directory '{}' doesn't exist", resourceDir.string());
        return {};
    }

    return { resourceDir };
}

std::filesystem::path Settings::GetBuildDir()
{
    CheckForUpdateAndPopulateNode();

    std::filesystem::path buildDir(node["BuildDirectory"].as<std::string>());
    if (!std::filesystem::exists(buildDir)) {
        LOG_ERROR("Build directory '{}' doesn't exist", buildDir.c_str());
        return {};
    }

    return buildDir;
}
}