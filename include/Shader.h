#pragma once

#include <string>

class Shader
{
public:
    Shader(const char* shaderFile);
    ~Shader();
    void Use();

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
};