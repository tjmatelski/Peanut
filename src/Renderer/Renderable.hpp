#pragma once

#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

namespace PEANUT {

struct Renderable {
    OpenglMesh mesh_;
    Material material_;
    const Shader* shader_;
};

}
