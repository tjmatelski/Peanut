#include "Renderer2D.hpp"

#include "../Settings.hpp"
#include "GLDebug.hpp"
#include "IndexBuffer.hpp"
#include "Renderer.hpp"
#include "TextureLibrary.hpp"
#include "VertexArray.hpp"

// external
#include <glad/glad.h>

// stl
#include <memory>

namespace PEANUT {
struct QuadRenderData {
    std::shared_ptr<VertexArray> vertexArray;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::unique_ptr<Shader> shader;
};

namespace {
    QuadRenderData s_quadRenderData;
}

void Renderer2D::Init()
{
    constexpr std::array vertices = {
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f // top left
    };
    constexpr std::array<unsigned int, 6> indices = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    s_quadRenderData.vertexArray = std::make_shared<VertexArray>();
    VertexBuffer quadIndexBuffer(vertices.size() * sizeof(decltype(vertices[0])), vertices.data());
    BufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    s_quadRenderData.vertexArray->AddBuffer(std::move(quadIndexBuffer), layout);
    s_quadRenderData.indexBuffer = std::make_shared<IndexBuffer>(indices.size(), indices.data());

    s_quadRenderData.shader = std::make_unique<Shader>(Settings::GetResourceDir() / "shaders" / "Generic2D.shader");
}

void Renderer2D::Destroy()
{
    s_quadRenderData.indexBuffer.reset();
    s_quadRenderData.vertexArray.reset();
    s_quadRenderData.shader.reset();
}

void Renderer2D::BeginScene(const OrthoCamera& camera)
{
    s_quadRenderData.shader->SetUniformMat4("view", camera.GetViewMatrix());
    s_quadRenderData.shader->SetUniformMat4("projection", camera.GetProjectionMatrix());
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec3& color)
{
    DrawQuad(transform, color, TextureLibrary::Load("textures/BlankSquare.png"));
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const Texture& texture)
{
    DrawQuad(transform, { 1.0f, 1.0f, 1.0f }, texture);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec3& color, const Texture& texture)
{
    s_quadRenderData.shader->SetUniformMat4("model", transform);
    s_quadRenderData.shader->SetUniformVec3("color", color);
    Renderer::Draw(*s_quadRenderData.vertexArray, *s_quadRenderData.indexBuffer, *s_quadRenderData.shader, texture);
}

} // namespace PEANUT
