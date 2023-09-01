#pragma once

#include "Scene.h"

namespace PEANUT {
class SceneSerializer {
public:
    static void Serialize(Scene& scene, const std::string& filepath);
    static void Deserialize(const std::string& file, Scene& scene);
};
} // namespace PEANUT
