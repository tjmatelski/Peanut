#pragma once

#include "../Math.h"

#include <string>

namespace PEANUT
{

class Shader
{
public:
    Shader(const char* shaderFile);
    ~Shader();
    void Use() const;
    void SetUniform4f(const char* name, const float a, const float b, const float c, const float d);
    void SetUniform1i(const char* name, const int i);
    void SetUniformMat4(const char* name, const glm::mat4& matrix);
    void SetUniformVec3(const char* name, const glm::vec3& vec);

private:
    struct ShaderSources
    {
        std::string vertex;
        std::string fragment;
    };
    unsigned int m_ShaderProgramID;

private:
    ShaderSources ParseShaderFile(const char* file);
    unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    unsigned int CompileShader(const unsigned int type, const std::string& shaderSource);
    int GetUniformLocation(const char* name);
};

}