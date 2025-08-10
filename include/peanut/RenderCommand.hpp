#pragma once

// peanut
#include "peanut/Material.hpp"
#include "peanut/Mesh.hpp"
#include "peanut/Shader.hpp"
#include "peanut/Texture.hpp"

// stl
#include <string>
#include <utility>
#include <vector>

namespace PEANUT {

struct RenderCommand {
    const Shader* p_shader_ { nullptr };
    const Mesh* p_mesh_ { nullptr };
    const Material* p_material_ { nullptr };
    std::vector<std::pair<std::string, const Texture*>> textures_ {};
    std::vector<std::pair<std::string, UniformValue>> uniforms_ {};
};
}
