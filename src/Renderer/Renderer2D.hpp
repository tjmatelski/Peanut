#pragma once

#include "OrthoCamera.hpp"
#include "Texture.hpp"
#include <peanut/Math.hpp>

namespace PEANUT {

class Renderer2D {
public:
    static void Init();
    static void Destroy();
    static void BeginScene(const OrthoCamera& camera);
    static void DrawQuad(const glm::mat4& transform, const glm::vec3& color);
    static void DrawQuad(const glm::mat4& transform, const Texture& texture);
    static void DrawQuad(const glm::mat4& transform, const glm::vec3& color, const Texture& texture);
};

} // namespace PEANUT
