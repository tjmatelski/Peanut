#include <Renderer/Shader.h>
#include <Log.h>
#include <glad/glad.h>

#include "GLDebug.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

Shader::Shader(const char* shaderFile)
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

Shader::ShaderSources Shader::ParseShaderFile(const char* file)
{
    std::ifstream inputStream(file);
    std::string line;
    std::stringstream ss[2];

    enum class StreamType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    if (!inputStream.is_open())
    {
        LOG_ERROR("Failed to open shader {0}", file);
    }
    else
    {
        StreamType type = StreamType::NONE;

        while (getline(inputStream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = StreamType::VERTEX;
                }
                if (line.find("fragment") != std::string::npos)
                {
                    type = StreamType::FRAGMENT;
                }
            }
            else
            {
                ss[static_cast<int>(type)] << line << '\n';
            }
        }
    }

    return {ss[static_cast<int>(StreamType::VERTEX)].str(), ss[static_cast<int>(StreamType::FRAGMENT)].str()};
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
    if (!success)
    {
        char infoLog[512];
        GLCALL(glGetProgramInfoLog(programID, 512, NULL, infoLog));
        LOG_ERROR("ERROR::SHADER::LINKING Info: {0}", infoLog);
    }
    GLCALL(glDeleteShader(vertexID));
    GLCALL(glDeleteShader(fragmentID));
    return programID;
}

unsigned int Shader::CompileShader(const unsigned int type, const std::string &shaderSource)
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
    if (!success)
    {
        char infoLog[512];
        GLCALL(glGetShaderInfoLog(shaderID, 512, NULL, infoLog));
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << "\nSource: " << shaderSource << std::endl;
        LOG_ERROR("ERROR::SHADER::TYPE {0}::COMPILATION_FAILED Info: {1}\nShaderSource:\n{2}", type, infoLog, shaderSource);
    }
    return shaderID;
}

void Shader::SetUniform4f(const char* name, const float a, const float b, const float c, const float d)
{
    int vertexColorLocation = glGetUniformLocation(m_ShaderProgramID, name);
    GL_CHECK_ERROR();
    if (vertexColorLocation == -1)
    {
        LOG_ERROR("Could not find uniform: {0}", name);
    }
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform4f(vertexColorLocation, a, b, c, d));
}

void Shader::SetUniform1i(const char* name, const int i)
{
    int vertexColorLocation = glGetUniformLocation(m_ShaderProgramID, name);
    GL_CHECK_ERROR();
    if (vertexColorLocation == -1)
    {
        LOG_ERROR("Could not find uniform: {0}", name);
    }
    GLCALL(glUseProgram(m_ShaderProgramID));
    GLCALL(glUniform1i(vertexColorLocation, i));
}