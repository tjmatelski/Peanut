#include "../Settings.h"
#include "GLDebug.h"
#include <Renderer/IndexBuffer.h>
#include <Renderer/Renderer.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/VertexArray.h>
#include <glad/glad.h>

#include <memory>

namespace PEANUT {
struct QuadRenderData {
    std::shared_ptr<VertexArray> vertexArray;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> blankTexture;
};

static QuadRenderData s_quadRenderData;

void Renderer2D::Init()
{
    constexpr float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f // top left
    };
    constexpr unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    s_quadRenderData.vertexArray = std::make_shared<VertexArray>();
    VertexBuffer quadIndexBuffer(sizeof(vertices), vertices);
    BufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    s_quadRenderData.vertexArray->AddBuffer(quadIndexBuffer, layout);
    s_quadRenderData.indexBuffer = std::make_shared<IndexBuffer>(6, indices);

    s_quadRenderData.shader = std::make_unique<Shader>(Settings::GetResourceDir() / "shaders" / "Generic2D.shader");
    s_quadRenderData.blankTexture = std::make_unique<Texture>(Settings::GetResourceDir() / "textures" / "BlankSquare.png");
}

void Renderer2D::BeginScene(const OrthoCamera& camera)
{
    s_quadRenderData.shader->SetUniformMat4("view", camera.GetViewMatrix());
    s_quadRenderData.shader->SetUniformMat4("projection", camera.GetProjectionMatrix());
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec3& color)
{
    DrawQuad(transform, color, *s_quadRenderData.blankTexture);
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
