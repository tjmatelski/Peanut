#include "Shader.hpp"

#include "../Util.hpp"
#include "GLDebug.hpp"
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>

// stl
#include <concepts>
#include <fstream>
#include <sstream>
#include <string>
#include <variant>

namespace {
template <class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};
}

namespace PEANUT {

Shader::Shader(const std::filesystem::path& shaderFile)
{
    ShaderSources shaderSources = ParseShaderFile(shaderFile);
    m_ShaderProgramID = CreateShaderProgram(shaderSources.vertex, shaderSources.fragment);
}

Shader::Shader(Shader&& other)
    : m_ShaderProgramID(other.m_ShaderProgramID)
{
    other.m_ShaderProgramID = 0;
}

Shader& Shader::operator=(Shader&& other)
{
    m_ShaderProgramID = other.m_ShaderProgramID;
    other.m_ShaderProgramID = 0;
    return *this;
}

Shader::~Shader()
{
    if (m_ShaderProgramID != 0) {
        GLCALL(glDeleteProgram(m_ShaderProgramID));
        m_ShaderProgramID = 0;
    }
}

void Shader::Use() const { GLCALL(glUseProgram(m_ShaderProgramID)); }

Shader::ShaderSources Shader::ParseShaderFile(const std::filesystem::path& file)
{
    std::ifstream inputStream(file);
    std::string line;
    std::array<std::stringstream, 2> ss;

    enum class StreamType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

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

    // Print attributes
    GLint count = 0;
    GLCALL(glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &count));
    LOG_DEBUG("Shader [{}] active attributes [{}]", programID, count);

    for (GLint i = 0; i < count; i++) {
        GLsizei length = 0; // name length
        GLint size = 0; // size of the variable
        GLenum type = 0; // type of the variable (float, vec3 or mat4, etc)
        std::array<GLchar, 256> name { '\0' };
        GLCALL(glGetActiveAttrib(programID, (GLuint)i, name.size(), &length, &size, &type, name.data()));

        LOG_DEBUG("Attribute [{}] type [{}] length [{}]", name.data(), type, length);
    }

    // Print Uniforms
    GLCALL(glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &count));
    LOG_DEBUG("Shader [{}] active uniforms [{}]", programID, count);

    for (GLint i = 0; i < count; i++) {
        GLsizei length = 0; // name length
        GLint size = 0; // size of the variable
        GLenum type = 0; // type of the variable (float, vec3 or mat4, etc)
        std::array<GLchar, 256> name { '\0' };
        glGetActiveUniform(programID, (GLuint)i, name.size(), &length, &size, &type, name.data());

        LOG_DEBUG("Uniform [{}] type [{}] length [{}]", name.data(), type, length);
    }

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
        LOG_ERROR("ERROR::SHADER::TYPE {0}::COMPILATION_FAILED Info: {1}\nShaderSource:\n{2}", type, infoLog.data(),
            shaderSource);
        throw "Failed to compile shader";
    }

    return shaderID;
}

void Shader::SetUniform(const Uniform& uniform) const
{
    const auto loc = GetUniformLocation(uniform.name.c_str());
    Use();
    std::visit(
        Overloaded { [&](std::same_as<bool> auto value) { GLCALL(glUniform1i(loc, static_cast<GLint>(value))); },
            [&](std::same_as<int> auto value) { GLCALL(glUniform1i(loc, value)); },
            [&](std::same_as<unsigned int> auto value) { GLCALL(glUniform1ui(loc, value)); },
            [&](std::same_as<float> auto value) { GLCALL(glUniform1f(loc, value)); },
            [&](std::same_as<glm::vec2> auto value) { GLCALL(glUniform2f(loc, value.x, value.y)); },
            [&](std::same_as<glm::vec3> auto value) { GLCALL(glUniform3f(loc, value.x, value.y, value.z)); },
            [&](std::same_as<glm::vec4> auto value) { GLCALL(glUniform4f(loc, value.x, value.y, value.z, value.w)); },
            [&](std::same_as<glm::mat2> auto value) {
                GLCALL(glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
            },
            [&](std::same_as<glm::mat3> auto value) {
                GLCALL(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
            },
            [&](std::same_as<glm::mat4> auto value) {
                GLCALL(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
            } },
        uniform.value);
}

int Shader::GetUniformLocation(const char* name) const
{
    int location = glGetUniformLocation(m_ShaderProgramID, name);
    GL_CHECK_ERROR();
    if (location == -1) {
        LOG_TRACE("Could not find uniform: {0}", name);
    }
    return location;
}

} // namespace PEANUT