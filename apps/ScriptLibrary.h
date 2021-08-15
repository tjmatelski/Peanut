#pragma once

#include <Log.h>

#include <dlfcn.h>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <memory>

namespace PEANUT
{

class ScriptLibrary
{
public:
    ~ScriptLibrary()
    {
        for (auto& [name, lib] : m_loadedLibs)
        {
            dlclose(lib.handle);
        }
    }

    std::unique_ptr<NativeScript> Load(const std::filesystem::path& script)
    {
        LOG_INFO("Loading script {}", script.string());
        auto fullPath = std::filesystem::canonical(script);
        auto search = m_loadedLibs.find(fullPath);
        if (search != m_loadedLibs.end())
        {
            return std::unique_ptr<NativeScript>(search->second.getScriptFunc());
        }

        std::string libName = "./build/res/scripts/lib" + script.stem().string() + ".so";
        LOG_DEBUG("Loading shared object {}", libName);
        ScriptLib lib;
        lib.handle = dlopen(libName.c_str(), RTLD_NOW);
        if (lib.handle == nullptr)
        {
            LOG_ERROR("Failed to load script library object {}, dlerror: {}", libName,  dlerror());
            return std::unique_ptr<NativeScript>();
        }

        typedef NativeScript *(*CreateScriptFunc)(void);
        CreateScriptFunc getScript = (CreateScriptFunc)dlsym(lib.handle, "GetScript");
        if (getScript == nullptr)
        {
            LOG_ERROR("Failed to load GetScript function from {}, dlerror: {}", libName, dlerror());
            dlclose(lib.handle);
            return std::unique_ptr<NativeScript>();
        }
        lib.getScriptFunc = getScript;

        m_loadedLibs.emplace(fullPath, lib);
        
        return std::unique_ptr<NativeScript>(lib.getScriptFunc());
    }

private:
    using LibHandle = void*;
    struct ScriptLib
    {
        //std::filesystem::path m_script;
        LibHandle handle;
        std::function<NativeScript *(void)> getScriptFunc;
    };
    std::map<std::filesystem::path, ScriptLib> m_loadedLibs;
};

}