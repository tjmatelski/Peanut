#include "TextureLibrary.hpp"
#include <peanut/Material.hpp>

namespace PEANUT {

auto Material::Default() -> Material
{
    Material mat;
    mat.SetUniform("material.shininess", 32.0f);
    mat.AddTexture(TextureLibrary::Load("textures/BlankSquare.png"));
    return mat;
}

void Material::SetUniform(std::string_view name, UniformValue value)
{
    if (auto it = uniforms_.find(name); it != uniforms_.end()) {
        it->second = value;
        return;
    }
    uniforms_.emplace(name, value);
}

void Material::AddTexture(Texture texture) { textures_.push_back(texture); }
}