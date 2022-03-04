#pragma once

#include "../include/Scene/NativeScript.h"

#include <dlfcn.h>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>

namespace PEANUT {

class ScriptLibrary {
public:
    static std::unique_ptr<NativeScript> Load(const std::filesystem::path& script);

private:
    static ScriptLibrary& GetInstance();
    ~ScriptLibrary();

    std::unique_ptr<NativeScript> LoadImpl(const std::filesystem::path& script);
    void BuildLibrary(const std::filesystem::path& script);

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