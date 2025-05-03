#include "Renderer.hpp"

#include "GLDebug.hpp"
#include <peanut/Component.hpp>
#include <peanut/Texture.hpp>

// external
#include <glad/glad.h>
#include <vector>

namespace {
static const float cubeVerts[] = {
    // clang-format off
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    // clang-format on
};

static const unsigned int cubeIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };

static constexpr float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
    -1.0f,

    -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
    1.0f,

    1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f,
    1.0f
};
}

namespace PEANUT {

Renderer::Renderer() { }

void Renderer::ClearColor(const float r, const float g, const float b, const float a)
{
    GLCALL(glClearColor(r, g, b, a));
}

void Renderer::ClearBuffers() { GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); }

void Renderer::ClearDepthBuffer() { GLCALL(glClear(GL_DEPTH_BUFFER_BIT)); }

void Renderer::EnableDepthTest() { GLCALL(glEnable(GL_DEPTH_TEST)); }

void Renderer::DisableDepthMask() { GLCALL(glDepthMask(GL_FALSE)); }

void Renderer::EnableDepthMask() { GLCALL(glDepthMask(GL_TRUE)); }

void Renderer::SetViewport(const int width, const int height) { GLCALL(glViewport(0, 0, width, height)); }

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader)
{
    vertexArray.Bind();
    indexBuffer.Bind();
    shader.Use();

    GLCALL(glDrawElements(GL_TRIANGLES, indexBuffer.GetElementCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::Draw(
    const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture)
{
    texture.Bind();
    Draw(vertexArray, indexBuffer, shader);
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader,
    const std::vector<Texture>& textures)
{
    int glTextureNumber = GL_TEXTURE0;
    for (const auto texture : textures) {
        GLCALL(glActiveTexture(glTextureNumber++));
        texture.Bind();
    }
    Draw(vertexArray, indexBuffer, shader);
}

void Renderer::Draw(const Model& model)
{
    for (const auto& renderable : model.GetRenderables()) {
        Draw(renderable);
    }
}

void Renderer::Draw(const Renderable& renderable)
{
    unsigned int glTextureNumber = 0;
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    renderable.shader_->Use();
    for (const auto texture : renderable.material_.Textures()) {
        if (texture.GetType() == Texture::Type::Diffuse) {
            renderable.shader_->SetUniform(
                { "material.diffuse[" + std::to_string(numDiffuse++) + "]", static_cast<int>(glTextureNumber) });
        }
        if (texture.GetType() == Texture::Type::Specular) {
            renderable.shader_->SetUniform(
                { "material.specular[" + std::to_string(numSpecular++) + "]", static_cast<int>(glTextureNumber) });
        }

        GLCALL(glActiveTexture(GL_TEXTURE0 + glTextureNumber++));
        texture.Bind();
    }
    for (const auto& [name, value] : renderable.material_.Uniforms()) {
        // TODO: Set Shinyness when constructing materials
        renderable.shader_->SetUniform({ name, value });
    }
    renderable.mesh_.GetVertexArray().Bind();
    renderable.mesh_.GetIndexBuffer().Bind();

    GLCALL(glDrawElements(GL_TRIANGLES, renderable.mesh_.GetIndexBuffer().GetElementCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::SetDirectionalLight(const DirectionalLight& dirLight, const Shader& shader)
{
    shader.SetUniform({ "dirLight.direction", dirLight.direction });
    shader.SetUniform({ "dirLight.ambient", dirLight.ambient });
    shader.SetUniform({ "dirLight.diffuse", dirLight.diffuse });
    shader.SetUniform({ "dirLight.specular", dirLight.specular });
}

void Renderer::SetPointLights(const std::vector<PointLight>& lights, const Shader& shader)
{
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].is_active", false });
    }
    unsigned int i = 0;
    for (const auto& light : lights) {
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].is_active", light.active });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].position", light.position });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].ambient", light.ambient });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].diffuse", light.diffuse });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].specular", light.specular });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].constant", light.constant });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].linear", light.linear });
        shader.SetUniform({ "pointLights[" + std::to_string(i) + "].quadratic", light.quadratic });
        ++i;
    }
}

void Renderer::SetSpotLight(const SpotLight& spotLight, Shader& shader)
{
    shader.SetUniform({ "spotLight.position", spotLight.position });
    shader.SetUniform({ "spotLight.direction", spotLight.direction });
    shader.SetUniform({ "spotLight.diffuse", spotLight.diffuse });
    shader.SetUniform({ "spotLight.specular", spotLight.specular });
    shader.SetUniform({ "spotLight.cutoff", glm::cos(glm::radians(spotLight.beginCutoff)) });
    shader.SetUniform({ "spotLight.outerCutoff", glm::cos(glm::radians(spotLight.endCutoff)) });
}

Mesh Renderer::GetCubeMesh()
{
    std::vector<Vertex> verts;
    std::vector<unsigned int> indices;
    for (int i = 0; i < (8 * 36); i += 8) {
        Vertex vert;
        vert.position = glm::vec3(cubeVerts[i + 0], cubeVerts[i + 1], cubeVerts[i + 2]);
        vert.normal = glm::vec3(cubeVerts[i + 3], cubeVerts[i + 4], cubeVerts[i + 5]);
        vert.texCoords = glm::vec2(cubeVerts[i + 6], cubeVerts[i + 7]);
        verts.push_back(vert);
    }
    for (unsigned int cubeIndice : cubeIndices) {
        indices.push_back(cubeIndice);
    }
    Mesh mesh(std::move(verts), std::move(indices));
    return mesh;
}

Mesh Renderer::GetQuadMesh()
{
    static constexpr std::array quad_data = {
        // clang-format off
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        // clang-format on
    };
    std::vector<Vertex> verts;
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5 };
    for (size_t i = 0; i < quad_data.size(); i += 4) {
        Vertex vert;
        vert.position = glm::vec3(quad_data[i + 0], quad_data[i + 1], 0);
        vert.normal = glm::vec3();
        vert.texCoords = glm::vec2(quad_data[i + 2], quad_data[i + 3]);
        verts.push_back(vert);
    }

    Mesh mesh(std::move(verts), std::move(indices));
    return mesh;
}

auto Renderer::GetSkyboxMesh() -> Mesh
{
    std::vector<Vertex> verts;
    std::vector<unsigned int> indicies;
    for (int i = 0; i < (3 * 36); i += 3) {
        Vertex vert;
        vert.position = glm::vec3(skyboxVertices[i + 0], skyboxVertices[i + 1], skyboxVertices[i + 2]);
        vert.normal = glm::vec3();
        vert.texCoords = glm::vec2();
        verts.push_back(vert);
    }
    for (int i = 0; i < 36; ++i) {
        indicies.push_back(cubeIndices[i]);
    }

    return Mesh(std::move(verts), std::move(indicies));
}

}