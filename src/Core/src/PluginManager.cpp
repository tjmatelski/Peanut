#include "PluginManager.hpp"
#include "Scene/NativeScript.h"
#include "Utils/Log.h"
#include <dlfcn.h>
#include <exception>
#include <filesystem>
#include <memory>

namespace PEANUT {

void PluginManager::LoadAll(const std::filesystem::path& path)
{
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".so") {
            try {
                m_plugins.push_back(Load(entry.path()));
            } catch (const std::exception& e) {
                LOG_ERROR("{}", e.what());
            }
        }
    }
}

void PluginManager::Reload(std::string_view name)
{
    const auto itr = std::find_if(m_plugins.begin(), m_plugins.end(), [name](const auto& plugin) { return name == plugin.name; });
    if (itr == m_plugins.end()) {
        LOG_ERROR("Failed to reload plugin: {}", name);
        return;
    }
    dlclose(itr->handle);
    try {
        *itr = Load(itr->path);
    } catch (const std::exception& e) {
        LOG_ERROR("{}", e.what());
    }
}

Plugin PluginManager::Load(const std::filesystem::path& lib)
{
    LOG_DEBUG("Loading plugin: {}", lib.c_str());
    dlerror(); // Clear any previous errors
    void* handle = dlopen(lib.c_str(), RTLD_LAZY);
    if (auto* error = dlerror()) {
        throw error;
    }
    auto* get_name = (const char* (*)())dlsym(handle, "plugin_name");
    if (auto* error = dlerror()) {
        throw error;
    }
    auto* get_component = (NativeScript * (*)()) dlsym(handle, "get_new_component");
    if (auto* error = dlerror()) {
        throw error;
    }

    Plugin p;
    p.name = get_name();
    p.handle = handle;
    p.getNewComponent = get_component;
    p.path = lib;
    return p;
}
}