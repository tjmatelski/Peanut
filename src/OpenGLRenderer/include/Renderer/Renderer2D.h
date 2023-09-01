#pragma once

#include "OrthoCamera.h"
#include "Texture.h"
#include "Utils/Math.h"

namespace PEANUT {

class Renderer2D {
public:
    static void Init();
    static void BeginScene(const OrthoCamera& camera);
    static void DrawQuad(const glm::mat4& transform, const glm::vec3& color);
    static void DrawQuad(const glm::mat4& transform, const Texture& texture);
    static void DrawQuad(const glm::mat4& transform, const glm::vec3& color, const Texture& texture);
};

} // namespace PEANUT
