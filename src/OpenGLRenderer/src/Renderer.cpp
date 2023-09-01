#include <Renderer/Renderer.h>

#include "GLDebug.h"
#include "Renderer/Texture.h"

#include <glad/glad.h>

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

static const unsigned int cubeIndices[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

static constexpr float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};
}

namespace PEANUT {

Renderer::Renderer() { }

void Renderer::ClearColor(const float r, const float g, const float b, const float a)
{
    GLCALL(glClearColor(r, g, b, a));
}

void Renderer::ClearBuffers()
{
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::EnableDepthTest()
{
    GLCALL(glEnable(GL_DEPTH_TEST));
}

void Renderer::DisableDepthMask()
{
    GLCALL(glDepthMask(GL_FALSE));
}

void Renderer::EnableDepthMask()
{
    GLCALL(glDepthMask(GL_TRUE));
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

    GLCALL(glDrawElements(GL_TRIANGLES, indexBuffer.GetElementCount(), GL_UNSIGNED_INT, 0));
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture)
{
    texture.Bind();
    Draw(vertexArray, indexBuffer, shader);
}

void Renderer::Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const std::vector<Texture>& textures)
{
    int glTextureNumber = GL_TEXTURE0;
    for (const auto texture : textures) {
        GLCALL(glActiveTexture(glTextureNumber++));
        texture.Bind();
    }
    Draw(vertexArray, indexBuffer, shader);
}

void Renderer::Draw(const Mesh& mesh, const Material material, Shader& shader)
{
    unsigned int glTextureNumber = 0;
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    for (const auto texture : material.GetTextures()) {
        if (texture.GetType() == Texture::Type::Diffuse) {
            shader.SetUniform1i("material.diffuse[" + std::to_string(numDiffuse++) + "]", glTextureNumber);
        }
        if (texture.GetType() == Texture::Type::Specular) {
            shader.SetUniform1i("material.specular[" + std::to_string(numSpecular++) + "]", glTextureNumber);
        }
        if (texture.GetType() != Texture::Type::CubeMap) {
            shader.SetUniform1f("material.shininess", material.GetShininess());
        }
        GLCALL(glActiveTexture(GL_TEXTURE0 + glTextureNumber++));
        texture.Bind();
    }
    Draw(mesh.GetVertexArray(), mesh.GetIndexBuffer(), shader);
}

void Renderer::Draw(const Model& model, Shader& shader)
{
    for (const auto& renderable : model.GetRenderables()) {
        Draw(renderable.mesh, renderable.material, shader);
    }
}

void Renderer::SetDirectionalLight(const DirectionalLight& dirLight, Shader& shader)
{
    shader.SetUniformVec3("dirLight.direction", dirLight.direction);
    shader.SetUniformVec3("dirLight.ambient", dirLight.ambient);
    shader.SetUniformVec3("dirLight.diffuse", dirLight.diffuse);
    shader.SetUniformVec3("dirLight.specular", dirLight.specular);
}

void Renderer::SetPointLights(const std::vector<PointLight>& lights, Shader& shader)
{
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i) {
        shader.SetUniform1b("pointLights[" + std::to_string(i) + "].is_active", false);
    }
    unsigned int i = 0;
    for (const auto& light : lights) {
        shader.SetUniform1b("pointLights[" + std::to_string(i) + "].is_active", light.active);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].position", light.position);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].ambient", light.ambient);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].diffuse", light.diffuse);
        shader.SetUniformVec3("pointLights[" + std::to_string(i) + "].specular", light.specular);
        shader.SetUniform1f("pointLights[" + std::to_string(i) + "].constant", light.constant);
        shader.SetUniform1f("pointLights[" + std::to_string(i) + "].linear", light.linear);
        shader.SetUniform1f("pointLights[" + std::to_string(i) + "].quadratic", light.quadratic);
        ++i;
    }
}

void Renderer::SetSpotLight(const SpotLight& spotLight, Shader& shader)
{
    shader.SetUniformVec3("spotLight.position", spotLight.position);
    shader.SetUniformVec3("spotLight.direction", spotLight.direction);
    shader.SetUniformVec3("spotLight.diffuse", spotLight.diffuse);
    shader.SetUniformVec3("spotLight.specular", spotLight.specular);
    shader.SetUniform1f("spotLight.cutoff", glm::cos(glm::radians(spotLight.beginCutoff)));
    shader.SetUniform1f("spotLight.outerCutoff", glm::cos(glm::radians(spotLight.endCutoff)));
}

Mesh Renderer::GetCubeMesh()
{
    std::vector<Mesh::Vertex> verts;
    std::vector<unsigned int> indicies;
    for (int i = 0; i < (8 * 36); i += 8) {
        Mesh::Vertex vert;
        vert.position = glm::vec3(cubeVerts[i + 0], cubeVerts[i + 1], cubeVerts[i + 2]);
        vert.normal = glm::vec3(cubeVerts[i + 3], cubeVerts[i + 4], cubeVerts[i + 5]);
        vert.texCoords = glm::vec2(cubeVerts[i + 6], cubeVerts[i + 7]);
        verts.push_back(vert);
    }
    for (int i = 0; i < 36; ++i) {
        indicies.push_back(cubeIndices[i]);
    }

    return Mesh(std::move(verts), std::move(indicies));
}

Mesh Renderer::GetSkyboxMesh()
{
    std::vector<Mesh::Vertex> verts;
    std::vector<unsigned int> indicies;
    for (int i = 0; i < (3 * 36); i += 3) {
        Mesh::Vertex vert;
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