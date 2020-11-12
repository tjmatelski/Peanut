#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

class Renderer
{
public:
    Renderer();
    static void ClearColor(const float r, const float g, const float b, const float a);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture);
};