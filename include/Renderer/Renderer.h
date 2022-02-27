#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

#include <vector>

namespace PEANUT {

class Renderer {
public:
    Renderer() = default;
    static void ClearColor(const float r, const float g, const float b, const float a);
    static void SetViewport(const int width, const int height);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const std::vector<Texture*>& textures);
};

}