#include "GLDebug.h"
#include <Renderer/Renderer.h>
#include <glad/glad.h>

namespace PEANUT {

void Renderer::ClearColor(const float r, const float g, const float b, const float a)
{
    GLCALL(glClearColor(r, g, b, a));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::SetViewport(const int width, const int height)
{
    GLCALL(glViewport(0, 0, width, height));
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader)
{
    vertexArray.Bind();
    indexBuffer.Bind();
    shader.Use();

    GLCALL(glDrawElements(GL_TRIANGLES, indexBuffer.GetElementCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture)
{
    texture.Bind();
    Draw(vertexArray, indexBuffer, shader);
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const std::vector<Texture*>& textures)
{
    int glTextureNumber = GL_TEXTURE0;
    for (const auto texture : textures) {
        GLCALL(glActiveTexture(glTextureNumber++));
        texture->Bind();
    }
    Draw(vertexArray, indexBuffer, shader);
}

}