#pragma once

#include "IndexBuffer.h"
#include "Lights.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"

#include <vector>

namespace PEANUT {

class Renderer {
public:
    static const int MAX_POINT_LIGHTS = 8; ///> Max Number of Point lights supported by shader. Make sure shader point light array matches in size.

    Renderer();
    static void ClearColor(const float r, const float g, const float b, const float a = 1.0f);
    static void ClearBuffers();
    static void EnableDepthTest();
    static Mesh GetCubeMesh();
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const Texture& texture);
    static void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader, const std::vector<Texture>& textures);
    static void Draw(const Mesh& mesh, const Material material, Shader& shader);
    static void Draw(const Model& model, Shader& shader);
    static void SetDirectionalLight(const DirectionalLight& dirLight, Shader& shader);
    static void SetPointLights(const std::vector<PointLight>& lights, Shader& shader);
    static void SetSpotLight(const SpotLight& spotLight, Shader& shader);
    static void SetViewport(const int width, const int height);
};

}
