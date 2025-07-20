#include "TextureLibrary.hpp"
#include <peanut/Material.hpp>

namespace PEANUT {

auto Material::Default() -> Material
{
    Material mat;
    mat.SetUniform("material.shininess", 32.0f);
    mat.AddDiffuseTexture(TextureLibrary::GetImage("textures/BlankSquare.png"));
    // TODO: Default specular?
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

void Material::AddDiffuseTexture(Texture* p_texture) { diffuse_textures_.push_back(p_texture); }

void Material::AddSpecularTexture(Texture* p_texture) { specular_textures_.push_back(p_texture); }
}