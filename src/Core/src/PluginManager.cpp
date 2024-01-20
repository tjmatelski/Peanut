#include "PluginManager.hpp"
#include "Scene/NativeScript.h"
#include "Utils/Log.h"
#include <dlfcn.h>
#include <filesystem>
#include <memory>

namespace PEANUT {

void PluginManager::LoadAll(std::filesystem::path path)
{
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == ".so") {
            LOG_DEBUG("Loading plugin: {}", entry.path().c_str());
            dlerror(); // Clear any previous errors
            void* handle = dlopen(entry.path().c_str(), RTLD_LAZY);
            if (auto* error = dlerror()) {
                LOG_ERROR("{}", error);
                continue;
            }
            auto* get_name = (const char* (*)())dlsym(handle, "plugin_name");
            if (auto* error = dlerror()) {
                LOG_ERROR("{}", error);
                continue;
            }
            auto* get_component = (NativeScript * (*)()) dlsym(handle, "get_new_component");
            if (auto* error = dlerror()) {
                LOG_ERROR("{}", error);
                continue;
            }

            Plugin p;
            p.name = get_name();
            p.handle = handle;
            p.getNewComponent = get_component;
            m_plugins.push_back(p);
        }
    }
}

}