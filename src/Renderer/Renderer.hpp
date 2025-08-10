#pragma once

#include "Lights.hpp"
#include "peanut/RenderCommand.hpp"
#include <peanut/IndexBuffer.hpp>
#include <peanut/Material.hpp>
#include <peanut/Mesh.hpp>
#include <peanut/Model.hpp>
#include <peanut/Shader.hpp>
#include <peanut/Texture.hpp>
#include <peanut/VertexArray.hpp>

// stl
#include <vector>

namespace PEANUT {

class Renderer {
public:
    static const int MAX_POINT_LIGHTS
        = 8; ///> Max Number of Point lights supported by shader. Make sure shader point light array matches in size.

    Renderer();
    static void ClearColor(const float r, const float g, const float b, const float a = 1.0f);
    static void ClearBuffers();
    static void ClearDepthBuffer();
    static void EnableDepthTest();
    static void DisableDepthTest();
    static void EnableDepthMask();
    static void DisableDepthMask();
    static Mesh GetCubeMesh();
    static Mesh GetQuadMesh();
    static auto GetSkyboxMesh() -> Mesh;
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
    static void Draw(
        const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader,
        const std::vector<Texture>& textures);
    static void Draw(const Model& model);
    static void Draw(const Renderable& renderable);
    static void Draw(const RenderCommand& renderable);
    static void SetDirectionalLight(const DirectionalLight& dirLight, const Shader& shader);
    static void SetPointLights(const std::vector<PointLight>& lights, const Shader& shader);
    static void SetSpotLight(const SpotLight& spotLight, Shader& shader);
    static void SetViewport(const int width, const int height);
};

}
