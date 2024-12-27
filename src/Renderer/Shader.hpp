#pragma once

#include <peanut/Math.hpp>

// stl
#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace PEANUT {

using UniformValue = std::variant<
    bool,
    int,
    unsigned int,
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    glm::mat2,
    glm::mat3,
    glm::mat4>;

struct Uniform {
    std::string name;
    UniformValue value;
};

class Shader {
public:
    Shader(const std::filesystem::path& shaderFile);
    ~Shader();
    void Use() const;
    void SetUniform(const Uniform& uniform) const;

private:
    struct ShaderSources {
        std::string vertex;
        std::string fragment;
    };
    unsigned int m_ShaderProgramID;
    const std::string m_shaderFile;
    std::unordered_map<std::string, int> m_cachedUniforms;
    std::unordered_set<std::string> m_nonExistantUniforms;

    ShaderSources ParseShaderFile(const std::filesystem::path& file);
    unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    unsigned int CompileShader(const unsigned int type, const std::string& shaderSource);
    int GetUniformLocation(const char* name) const;
};

}
