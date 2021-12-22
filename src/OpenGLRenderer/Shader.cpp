#include <Log.h>
#include <Renderer/Shader.h>
#include <Util.h>
#include <glad/glad.h>

#include "GLDebug.h"

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
    std::stringstream ss[2];

    enum class StreamType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    if (!inputStream.is_open()) {
        LOG_ERROR("Failed to open shader {0}", file.c_str());
        PN_ASSERT(false);
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
        char infoLog[512];
        GLCALL(glGetProgramInfoLog(programID, 512, NULL, infoLog));
        LOG_ERROR("ERROR::SHADER::LINKING Info: {0}", infoLog);
        PN_ASSERT(false);
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
    GLCALL(glShaderSource(shaderID, 1, &source, NULL));
    GLCALL(glCompileShader(shaderID));
    // // // Check for shader compile errors
    int success = 0;
    GLCALL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));
    if (!success) {
        char infoLog[512];
        GLCALL(glGetShaderInfoLog(shaderID, 512, NULL, infoLog));
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << "\nSource: " << shaderSource << std::endl;
        LOG_ERROR("ERROR::SHADER::TYPE {0}::COMPILATION_FAILED Info: {1}\nShaderSource:\n{2}", type, infoLog, shaderSource);
        PN_ASSERT(false);
    }
    return shaderID;
}

int Shader::GetUniformLocation(const char* name)
{
    int location = glGetUniformLocation(m_ShaderProgramID, name);
    GL_CHECK_ERROR();
    if (location == -1) {
        LOG_ERROR("Could not find uniform: {0}", name);
    }
    return location;
}

void Shader::SetUniform4f(const char* name, const float a, const float b, const float c, const float d)
{
    int uniformLocation = GetUniformLocation(name);
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform4f(uniformLocation, a, b, c, d));
}

void Shader::SetUniform1i(const char* name, const int i)
{
    int uniformLocation = GetUniformLocation(name);
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform1i(uniformLocation, i));
}

void Shader::SetUniformMat4(const char* name, const glm::mat4& matrix)
{
    int uniformLocation = GetUniformLocation(name);
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformVec3(const char* name, const glm::vec3& vec)
{
    int uniformLocation = GetUniformLocation(name);
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform3f(uniformLocation, vec.x, vec.y, vec.z));
}

} // namespace PEANUT