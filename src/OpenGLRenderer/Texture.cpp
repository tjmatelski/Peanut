#include <Renderer/Texture.h>

#include "../stb_image.h"
#include "GLDebug.h"
#include <Log.h>

#include <glad/glad.h>

#include <algorithm>
#include <array>
#include <limits>

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
    constexpr int width = 2;
    constexpr int height = 2;
    std::array<unsigned char, width * height * 3> data;
    std::fill(data.begin(), data.end(), std::numeric_limits<unsigned char>::max());
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data()));
}

Texture::Texture(const std::string& file, const Type type)
    : Texture(file.c_str(), type)
{
}

Texture::Texture(const Texture& other, const Type type)
    : m_ID(other.m_ID)
    , m_type(type)
{
}

Texture::Texture(const char* textureFile, const Type type)
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
    // load and generate the texture
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    if (data) {
        switch (nrChannels) {
        case 3:
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
            break;
        case 4:
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            break;
        default:
            LOG_ERROR("Texture does not currently support a texture with {0} channels", nrChannels);
        }
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
    } else {
        LOG_ERROR("Failed to load texture from file '{0}'\n\tSTB: {1}",
            textureFile, stbi_failure_reason());
    }
    stbi_image_free(data);
}

void Texture::Bind() const
{
    GLCALL(glBindTexture(GL_TEXTURE_2D, m_ID));
}

}
