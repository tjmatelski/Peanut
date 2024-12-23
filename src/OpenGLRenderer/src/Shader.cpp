#include <Renderer/Shader.h>

#include "GLDebug.h"
#include <Utils/Log.h>
#include <Utils/Util.h>

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace PEANUT {

Shader::Shader(const std::filesystem::path& shaderFile)
{
    ShaderSources shaderSources = ParseShaderFile(shaderFile);
    m_ShaderProgramID = CreateShaderProgram(shaderSources.vertex, shaderSources.fragment);
}

Shader::~Shader()
{
    GLCALL(glDeleteProgram(m_ShaderProgramID));
}

void Shader::Use() const
{
    GLCALL(glUseProgram(m_ShaderProgramID));
}

Shader::ShaderSources Shader::ParseShaderFile(const std::filesystem::path& file)
{
    std::ifstream inputStream(file);
    std::string line;
    std::array<std::stringstream, 2> ss;

    enum class StreamType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    if (!inputStream.is_open()) {
        LOG_ERROR("Failed to open shader {0}", file.c_str());
        throw "Faild to open shader";
    } else {
        StreamType type = StreamType::NONE;

        while (getline(inputStream, line)) {
            if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = StreamType::VERTEX;
                }
                if (line.find("fragment") != std::string::npos) {
                    type = StreamType::FRAGMENT;
                }
            } else {
                ss[static_cast<int>(type)] << line << '\n';
            }
        }
    }

    return { ss[static_cast<int>(StreamType::VERTEX)].str(), ss[static_cast<int>(StreamType::FRAGMENT)].str() };
}

unsigned int Shader::CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
    unsigned int programID, vertexID, fragmentID;
    vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
    fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    programID = glCreateProgram();
    GL_CHECK_ERROR();
    GLCALL(glAttachShader(programID, vertexID));
    GLCALL(glAttachShader(programID, fragmentID));
    GLCALL(glLinkProgram(programID));
    GLCALL(glValidateProgram(programID));
    // // // Check for errors
    int success = 0;
    GLCALL(glGetProgramiv(programID, GL_LINK_STATUS, &success));
    if (!success) {
        std::array<char, 512> infoLog;
        GLCALL(glGetProgramInfoLog(programID, infoLog.size(), nullptr, infoLog.data()));
        LOG_ERROR("ERROR::SHADER::LINKING Info: {0}", infoLog.data());
        throw "Failed to link shader";
    }
    GLCALL(glDeleteShader(vertexID));
    GLCALL(glDeleteShader(fragmentID));
    return programID;
}

unsigned int Shader::CompileShader(const unsigned int type, const std::string& shaderSource)
{
    unsigned int shaderID;
    const char* source = shaderSource.c_str();
    shaderID = glCreateShader(type);
    GL_CHECK_ERROR();
    GLCALL(glShaderSource(shaderID, 1, &source, nullptr));
    GLCALL(glCompileShader(shaderID));
    // // // Check for shader compile errors
    int success = 0;
    GLCALL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));
    if (!success) {
        std::array<char, 512> infoLog;
        GLCALL(glGetShaderInfoLog(shaderID, infoLog.size(), nullptr, infoLog.data()));
        LOG_ERROR("ERROR::SHADER::TYPE {0}::COMPILATION_FAILED Info: {1}\nShaderSource:\n{2}", type, infoLog.data(), shaderSource);
        throw "Failed to compile shader";
    }
    return shaderID;
}

int Shader::GetUniformLocation(const char* name) const
{
    int location = glGetUniformLocation(m_ShaderProgramID, name);
    GL_CHECK_ERROR();
    if (location == -1) {
        LOG_ERROR("Could not find uniform: {0}", name);
    }
    return location;
}

void Shader::SetUniform1b(const std::string& name, const bool b) const
{
    SetUniform1i(name, static_cast<int>(b));
}

void Shader::SetUniform1f(const std::string& name, const float f) const
{
    int uniformLocation = GetUniformLocation(name.c_str());
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform1f(uniformLocation, f));
}

void Shader::SetUniform4f(const std::string& name, const float a, const float b, const float c, const float d) const
{
    int uniformLocation = GetUniformLocation(name.c_str());
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform4f(uniformLocation, a, b, c, d));
}

void Shader::SetUniform1i(const std::string& name, const int i) const
{
    int uniformLocation = GetUniformLocation(name.c_str());
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform1i(uniformLocation, i));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) const
{
    int uniformLocation = GetUniformLocation(name.c_str());
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformVec3(const std::string& name, const glm::vec3& vec) const
{
    int uniformLocation = GetUniformLocation(name.c_str());
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform3f(uniformLocation, vec.x, vec.y, vec.z));
}

} // namespace PEANUT