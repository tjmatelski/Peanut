#pragma once

#include "IndexBuffer.hpp"
#include "Lights.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

// stl
#include <vector>

namespace PEANUT {

class Renderer {
public:
    static const int MAX_POINT_LIGHTS = 8; ///> Max Number of Point lights supported by shader. Make sure shader point light array matches in size.

    Renderer();
    static void ClearColor(const float r, const float g, const float b, const float a = 1.0f);
    static void ClearBuffers();
    static void EnableDepthTest();
    static void DisableDepthMask();
    static void EnableDepthMask();
    static Mesh GetCubeMesh();
    static OpenglMesh GetSkyboxMesh();
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const std::vector<Texture>& textures);
    static void Draw(const OpenglMesh& mesh, const Material material, const Shader& shader);
    static void Draw(const Model& model, const Shader& shader);
    static void SetDirectionalLight(const DirectionalLight& dirLight, Shader& shader);
    static void SetPointLights(const std::vector<PointLight>& lights, Shader& shader);
    static void SetSpotLight(const SpotLight& spotLight, Shader& shader);
    static void SetViewport(const int width, const int height);
};

}
