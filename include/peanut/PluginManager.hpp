#pragma once

#include <peanut/Entity.hpp>
#include <peanut/NativeScript.hpp>
#include <peanut/Scene.hpp>

// stl
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace PEANUT {

struct Plugin {
    std::string name;
    void* handle;
    std::function<NativeScript*()> getNewComponent;
    std::filesystem::path path;
};

class PluginManager {
public:
    void LoadAll(const std::filesystem::path& directory);
    void Reload(std::string_view name);
    [[nodiscard]] auto Plugins() const -> const std::vector<Plugin>& { return m_plugins; }

private:
    Plugin Load(const std::filesystem::path& lib);
    std::vector<Plugin> m_plugins;
};

}