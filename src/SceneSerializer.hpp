#pragma once

#include <peanut/Scene.hpp>

namespace PEANUT {
class SceneSerializer {
public:
    static void Serialize(Scene& scene, const std::string& filepath, const std::vector<std::string>& plugins);
    static void Deserialize(const std::string& file, Scene& scene, const std::vector<std::string>& plugins);
};
} // namespace PEANUT
