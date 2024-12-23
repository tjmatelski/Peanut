#pragma once

#include "Utils/Math.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace PEANUT {

class Shader {
public:
    Shader(const std::filesystem::path& shaderFile);
    ~Shader();
    void Use() const;
    void SetUniform1b(const std::string& name, const bool b) const;
    void SetUniform1f(const std::string& name, const float f) const;
    void SetUniform4f(const std::string& name, const float a, const float b, const float c, const float d) const;
    void SetUniform1i(const std::string& name, const int i) const;
    void SetUniformMat4(const std::string& name, const glm::mat4& matrix) const;
    void SetUniformVec3(const std::string& name, const glm::vec3& vec) const;

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
