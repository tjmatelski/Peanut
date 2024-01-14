#pragma once

#include "Scene/NativeScript.h"
#include <Scene/Entity.h>

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace PEANUT {

struct Plugin {
    std::string name;
    void* handle;
    std::function<void(Entity)> addToEntity;
    std::function<bool(Entity)> entityHasComponent;
    std::function<void(Entity)> removeFromEntity;
    std::function<NativeScript*(Entity)> getAsNativeScript;
};

class PluginManager {
public:
    void LoadAll(std::filesystem::path directory);
    [[nodiscard]] auto Plugins() const -> const std::vector<Plugin>& { return m_plugins; }

private:
    std::vector<Plugin> m_plugins;
};

}