#include <Renderer/Texture.h>

#include "GLDebug.h"
#include "Utils/stb_image.h"
#include <Utils/Log.h>

#include <glad/glad.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <limits>

namespace {
void LoadTexImage2D(int glType, const std::filesystem::path& file)
{
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        switch (nrChannels) {
        case 3:
            GLCALL(glTexImage2D(glType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
            break;
        case 4:
            GLCALL(glTexImage2D(glType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            break;
        default:
            LOG_ERROR("Texture does not currently support a texture with {0} channels", nrChannels);
        }
        GLCALL(glGenerateMipmap(glType));
    } else {
        LOG_ERROR("Failed to load texture from file '{0}'\n\tSTB: {1}",
            file.c_str(), stbi_failure_reason());
    }
    stbi_image_free(data);
}

void Load2DTexture(const std::string& file)
{
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    stbi_set_flip_vertically_on_load(true);
    LoadTexImage2D(GL_TEXTURE_2D, file);
}

void LoadCubeMap(const std::filesystem::path& directory)
{
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    stbi_set_flip_vertically_on_load(false);
    if (std::filesystem::exists(directory / "right.jpg")) {
        LoadTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, directory / "right.jpg");
    } else {
        LOG_ERROR("Failed to find cubemap file '{}'", std::filesystem::path { directory / "right.jpg" }.c_str());
    }
    if (std::filesystem::exists(directory / "left.jpg")) {
        LoadTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, directory / "left.jpg");
    } else {
        LOG_ERROR("Failed to find cubemap file '{}'", std::filesystem::path { directory / "left.jpg" }.c_str());
    }
    if (std::filesystem::exists(directory / "top.jpg")) {
        LoadTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, directory / "top.jpg");
    } else {
        LOG_ERROR("Failed to find cubemap file '{}'", std::filesystem::path { directory / "top.jpg" }.c_str());
    }
    if (std::filesystem::exists(directory / "bottom.jpg")) {
        LoadTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, directory / "bottom.jpg");
    } else {
        LOG_ERROR("Failed to find cubemap file '{}'", std::filesystem::path { directory / "bottom.jpg" }.c_str());
    }
    if (std::filesystem::exists(directory / "front.jpg")) {
        LoadTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, directory / "front.jpg");
    } else {
        LOG_ERROR("Failed to find cubemap file '{}'", std::filesystem::path { directory / "front.jpg" }.c_str());
    }
    if (std::filesystem::exists(directory / "back.jpg")) {
        LoadTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, directory / "back.jpg");
    } else {
        LOG_ERROR("Failed to find cubemap file '{}'", std::filesystem::path { directory / "back.jpg" }.c_str());
    }
}
}

namespace PEANUT {

Texture::Texture(const Type type)
    : m_ID(0)
    , m_type(type)
{
    GLCALL(glGenTextures(1, &m_ID));
    Bind();
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    constexpr int width = 1024;
    constexpr int height = 1024;
    std::array<unsigned char, width * height * 3> data;
    std::fill(data.begin(), data.end(), std::numeric_limits<unsigned char>::max());
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data()));
}

Texture::Texture(const Texture& other, const Type type)
    : m_ID(other.m_ID)
    , m_type(type)
{
}

Texture::Texture(const std::string& file, const Type type)
    : m_ID(0)
    , m_type(type)
{
    GLCALL(glGenTextures(1, &m_ID));
    Bind();
    if (m_type == Type::CubeMap) {
        LoadCubeMap(file);
    } else {
        Load2DTexture(file);
    }
}

void Texture::Bind() const
{
    if (m_type == Type::CubeMap) {
        GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID));
    } else {
        GLCALL(glBindTexture(GL_TEXTURE_2D, m_ID));
    }
}

}
