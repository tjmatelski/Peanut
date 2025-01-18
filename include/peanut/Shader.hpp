#pragma once

#include <peanut/Math.hpp>

// stl
#include <filesystem>
#include <string>
#include <string_view>
#include <variant>

namespace PEANUT {

using UniformValue
    = std::variant<bool, int, unsigned int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

struct Uniform {
    std::string name;
    UniformValue value;
};

class Shader {
public:
    Shader(const std::filesystem::path& shaderFile);
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other);
    Shader& operator=(Shader&& other);
    ~Shader();

    void Use() const;
    void SetUniform(const Uniform& uniform) const;

private:
    struct ShaderSources {
        std::string vertex;
        std::string fragment;
    };
    unsigned int m_ShaderProgramID;

    ShaderSources ParseShaderFile(const std::filesystem::path& file);
    unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    unsigned int CompileShader(const unsigned int type, const std::string& shaderSource);
    int GetUniformLocation(const char* name) const;
};

}

template <> struct std::hash<PEANUT::Uniform> {
    auto operator()(const PEANUT::Uniform& s) const noexcept { return std::hash<std::string> {}(s.name); }
    auto operator()(std::string_view name) const noexcept { return std::hash<std::string_view> {}(name); }
};

struct UniformEqualTo {
    bool operator()(const PEANUT::Uniform& u1, const PEANUT::Uniform& u2) { return u1.name == u2.name; }
};
