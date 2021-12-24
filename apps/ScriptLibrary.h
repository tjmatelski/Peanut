#pragma once

#include "../include/Scene/NativeScript.h"
#include "../src/Settings.h"
#include <Log.h>

#include <dlfcn.h>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace PEANUT {

class ScriptLibrary {
public:
    ~ScriptLibrary()
    {
        for (auto& [name, lib] : m_loadedLibs) {
            dlclose(lib.handle);
        }
    }

    static ScriptLibrary& GetInstance()
    {
        static ScriptLibrary sl;
        return sl;
    }

    static std::unique_ptr<NativeScript> Load(const std::filesystem::path& script)
    {
        return GetInstance().LoadImpl(script);
    }

private:
    std::unique_ptr<NativeScript> LoadImpl(const std::filesystem::path& script)
    {
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
        std::filesystem::path libObj = Settings::GetBuildDir() / "res" / "scripts" / libName;
        LOG_DEBUG("Loading shared object {}", libObj.c_str());
        ScriptLib lib;
        lib.libObject = libObj;
        lib.lastWriteTime = std::filesystem::last_write_time(lib.libObject);
        lib.handle = dlopen(libObj.c_str(), RTLD_NOW);
        if (lib.handle == nullptr) {
            LOG_ERROR("Failed to load script library object {}, dlerror: {}", libName, dlerror());
            return std::unique_ptr<NativeScript>();
        }

        typedef NativeScript* (*CreateScriptFunc)(void);
        CreateScriptFunc getScript = (CreateScriptFunc)dlsym(lib.handle, "GetScript");
        if (getScript == nullptr) {
            LOG_ERROR("Failed to load GetScript function from {}, dlerror: {}", libName, dlerror());
            dlclose(lib.handle);
            return std::unique_ptr<NativeScript>();
        }
        lib.getScriptFunc = getScript;

        m_loadedLibs.emplace(fullPath, lib);

        return std::unique_ptr<NativeScript>(lib.getScriptFunc());
    }

    using LibHandle = void*;
    struct ScriptLib {
        std::filesystem::path libObject;
        std::filesystem::file_time_type lastWriteTime;
        LibHandle handle;
        std::function<NativeScript*(void)> getScriptFunc;
    };
    std::map<std::filesystem::path, ScriptLib> m_loadedLibs;
};

}