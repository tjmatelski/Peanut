#include <Renderer.h>
#include <glad/glad.h>
#include "GLDebug.h"

Renderer::Renderer() {}

void Renderer::ClearColor(const float r, const float g, const float b, const float a)
{
    GLCALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray &vertexArray, const IndexBuffer &indexBuffer, const Shader &shader)
{
    vertexArray.Bind();
    indexBuffer.Bind();
    shader.Use();

    GLCALL(glDrawElements(GL_TRIANGLES, indexBuffer.GetElementCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture)
{
    texture.Bind();
    Draw(vertexArray, indexBuffer, shader);
}