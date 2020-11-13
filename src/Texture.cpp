#include <Texture.h>
#include <glad/glad.h>
#include <Log.h>
#include "GLDebug.h"
#include "stb_image.h"

Texture::Texture(const char *textureFile) : m_ID(0)
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
    unsigned char *data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    if (data)
    {
        switch (nrChannels)
        {
        case 3:
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
            break;
        case 4:
            GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            break;
        default:
            LOG_ERROR("Texture does not currently support a texture with {0} channels", nrChannels);
        }
        GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        LOG_INFO("Failed to load texture from file '{0}'", textureFile);
    }
    stbi_image_free(data);
}

void Texture::Bind() const 
{
    GLCALL(glBindTexture(GL_TEXTURE_2D, m_ID));
}