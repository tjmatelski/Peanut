#include "PluginManager.hpp"
#include "Utils/Log.h"
#include <dlfcn.h>
#include <filesystem>

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
            auto* add_to_entity = (void (*)(Entity))dlsym(handle, "add_to_entity");
            if (auto* error = dlerror()) {
                LOG_ERROR("{}", error);
                continue;
            }
            auto* remove_from_entity = (void (*)(Entity))dlsym(handle, "remove_from_entity");
            if (auto* error = dlerror()) {
                LOG_ERROR("{}", error);
                continue;
            }
            auto* entity_has_component = (bool (*)(Entity))dlsym(handle, "entity_has_component");
            if (auto* error = dlerror()) {
                LOG_ERROR("{}", error);
                continue;
            }

            Plugin p;
            p.name = get_name();
            p.handle = handle;
            p.addToEntity = add_to_entity;
            p.entityHasComponent = entity_has_component;
            p.removeFromEntity = remove_from_entity;
            m_plugins.push_back(p);
        }
    }
}

}