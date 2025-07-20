#pragma once

#include <peanut/Shader.hpp>
#include <peanut/Texture.hpp>

// stl
#include <functional>
#include <unordered_map>
#include <vector>

namespace PEANUT {

class Material {
public:
    [[nodiscard]] static auto Default() -> Material;

    void SetUniform(std::string_view name, UniformValue value);
    void AddDiffuseTexture(Texture* p_texture);
    void AddSpecularTexture(Texture* p_texture);

    [[nodiscard]] auto Uniforms() -> auto& { return uniforms_; }
    [[nodiscard]] auto Uniforms() const -> const auto& { return uniforms_; }
    [[nodiscard]] auto DiffuseTextures() const -> const auto& { return diffuse_textures_; }
    [[nodiscard]] auto SpecularTextures() const -> const auto& { return specular_textures_; }

private:
    struct StringHash {
        using hash_type = std::hash<std::string_view>;
        using is_transparent = void;

        std::size_t operator()(const char* str) const { return hash_type {}(str); }
        std::size_t operator()(std::string_view str) const { return hash_type {}(str); }
        std::size_t operator()(std::string const& str) const { return hash_type {}(str); }
    };
    using UniformMap = std::unordered_map<std::string, UniformValue, StringHash, std::equal_to<>>;
    UniformMap uniforms_;
    std::vector<Texture*> diffuse_textures_;
    std::vector<Texture*> specular_textures_;
};

}
