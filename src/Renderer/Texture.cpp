#include <peanut/Texture.hpp>

// peanut
#include "../stb_image.hpp"
#include "GLDebug.hpp"
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>

// stl
#include <filesystem>

namespace {
GLint ToGLType(PEANUT::Texture::Filter filter)
{
    switch (filter) {
    case PEANUT::Texture::Filter::LINEAR:
        return GL_LINEAR;
    case PEANUT::Texture::Filter::NEAREST:
        return GL_NEAREST;
    }
    return {};
}

GLint ToGLType(PEANUT::Texture::Wrapping wrapping)
{
    switch (wrapping) {
    case PEANUT::Texture::Wrapping::REPEAT:
        return GL_REPEAT;
    case PEANUT::Texture::Wrapping::MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;
    case PEANUT::Texture::Wrapping::CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
    case PEANUT::Texture::Wrapping::CLAMP_TO_BORDER:
        return GL_CLAMP_TO_BORDER;
    }
    return {};
}

GLint ToGLType(PEANUT::Texture::Type type)
{
    switch (type) {
    case PEANUT::Texture::Type::COLOR:
        return GL_RGB;
    case PEANUT::Texture::Type::DEPTH:
        return GL_DEPTH_COMPONENT;
    }
    return {};
}

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
        LOG_TRACE("Generated Mipmap");
    } else {
        LOG_ERROR("Failed to load texture from file '{0}'\n\tSTB: {1}", file.c_str(), stbi_failure_reason());
    }
    stbi_image_free(data);
}

void Load2DTexture(const std::filesystem::path& file)
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

Texture Texture::MakeTextureImage(std::filesystem::path file)
{
    Texture tex;
    if (!std::filesystem::exists(file)) {
        LOG_ERROR("File [{}] does not exist", file.string());
    }
    Load2DTexture(file);
    return tex;
}

Texture Texture::MakeTextureCubeMap(std::filesystem::path dir)
{
    Texture tex;
    if (!std::filesystem::exists(dir)) {
        LOG_ERROR("Cubemap directory [{}] does not exist", dir.string());
    }
    LoadCubeMap(dir);
    return tex;
}

Texture::Texture()
    : id_(0)
    , config_()
    , is_cube_map_(false)
{
    GLCALL(glGenTextures(1, &id_));
    LOG_TRACE("Generated texture: {}", id_);
    Bind();
}

Texture::Texture(Config config, unsigned char* p_data)
    : Texture()
{
    config_ = config;
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLType(config_.wrapping_)));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLType(config_.wrapping_)));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLType(config_.filter_)));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLType(config_.filter_)));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, ToGLType(config_.type_), config_.width_, config_.height_, 0,
        ToGLType(config_.type_), GL_UNSIGNED_BYTE, p_data));
}

Texture::Texture(Texture&& other) { *this = std::move(other); }

Texture& Texture::operator=(Texture&& other)
{
    id_ = other.id_;
    config_ = other.config_;
    is_cube_map_ = other.is_cube_map_;

    other.id_ = 0;
    other.config_ = {};
    other.is_cube_map_ = false;

    return *this;
}

Texture::~Texture()
{
    if (id_ != 0) {
        LOG_TRACE("Deleting Texture: [{}]", id_);
        GLCALL(glDeleteTextures(1, &id_));
    }
}

void Texture::Bind() const
{
    if (is_cube_map_) {
        GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, id_));
    } else {
        GLCALL(glBindTexture(GL_TEXTURE_2D, id_));
    }
}

}
