#include <ScriptLibrary.h>

#include "../src/Settings.h"
#include <Log.h>

#include <dlfcn.h>

namespace PEANUT {

ScriptLibrary::~ScriptLibrary()
{
    for (auto& [name, lib] : m_loadedLibs) {
        dlclose(lib.handle);
    }
}

ScriptLibrary& ScriptLibrary::GetInstance()
{
    static ScriptLibrary sl;
    return sl;
}

std::unique_ptr<NativeScript> ScriptLibrary::Load(const std::filesystem::path& script)
{
    return GetInstance().LoadImpl(script);
}

void ScriptLibrary::BuildLibrary(const std::filesystem::path& script)
{
    if (!std::filesystem::exists(script)) {
        LOG_ERROR("Script file doesn't exist! File: {}", script.c_str());
        return;
    }

    auto pathToSource = script.parent_path();
    auto pathToBuild = Settings::GetApplicationDir() / "script-build";

    if (std::filesystem::exists(pathToBuild)) {
        LOG_DEBUG("Removing existing build directory '{}'", pathToBuild.c_str());
        std::filesystem::remove_all(pathToBuild);
    }

    std::string configureCommand = "cmake -S ";
    configureCommand += pathToSource.string();
    configureCommand += " -B " + pathToBuild.string();
    configureCommand += " -DPEANUT_INCLUDE_DIR=" + (Settings::GetApplicationDir() / "include").string();

    LOG_DEBUG("Configuring script with command '{}'", configureCommand);

    auto result = std::system(configureCommand.c_str());
    if (result != 0) {
        LOG_ERROR("Failed to configure script {}", script.c_str());
        return;
    }

    std::string buildCommand = "cmake --build " + pathToBuild.string();

    LOG_DEBUG("Building script with command '{}'", buildCommand);

    result = std::system(buildCommand.c_str());
    if (result != 0) {
        LOG_ERROR("Failed to build script {}", script.c_str());
        return;
    }

    std::string installCommand = "cmake --install " + pathToBuild.string();
    installCommand += " --prefix " + Settings::GetApplicationDir().string();

    LOG_DEBUG("Installing script with command '{}'", installCommand);

    result = std::system(installCommand.c_str());
    if (result != 0) {
        LOG_ERROR("Failed to install script {}", script.c_str());
        return;
    }

    if (std::filesystem::exists(pathToBuild)) {
        LOG_DEBUG("Removing existing build directory '{}'", pathToBuild.c_str());
        std::filesystem::remove_all(pathToBuild);
    }
}

std::unique_ptr<NativeScript> ScriptLibrary::LoadImpl(const std::filesystem::path& script)
{
    BuildLibrary(script);

    LOG_INFO("Loading script {}", script.string());
    auto fullPath = std::filesystem::canonical(script);
    auto search = m_loadedLibs.find(fullPath);
    if (search != m_loadedLibs.end()) {
        if (search->second.lastWriteTime != std::filesystem::last_write_time(fullPath)) {
            dlclose(search->second.handle);
            m_loadedLibs.erase(search);
        } else {
            return std::unique_ptr<NativeScript>(search->second.getScriptFunc());
        }
    }

    std::string libName = "lib" + script.stem().string() + ".so";
    std::filesystem::path libObj = Settings::GetApplicationDir() / "lib" / libName;
    LOG_DEBUG("Loading shared object {}", libObj.c_str());
    ScriptLib lib;
    lib.libObject = libObj;
    lib.lastWriteTime = std::filesystem::last_write_time(lib.libObject);
    lib.handle = dlopen(libObj.c_str(), RTLD_NOW);
    if (lib.handle == nullptr) {
        LOG_ERROR("Failed to load script library object {}, dlerror: {}", libName, dlerror());
        return {};
    }

    using CreateScriptFunc = NativeScript* (*)();
    auto getScript = (CreateScriptFunc)dlsym(lib.handle, "GetScript");
    if (getScript == nullptr) {
        LOG_ERROR("Failed to load GetScript function from {}, dlerror: {}", libName, dlerror());
        dlclose(lib.handle);
        return {};
    }
    lib.getScriptFunc = getScript;

    m_loadedLibs.emplace(fullPath, lib);

    return std::unique_ptr<NativeScript>(lib.getScriptFunc());
}

}
