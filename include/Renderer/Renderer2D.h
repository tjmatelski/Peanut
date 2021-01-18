#pragma once

#include "../Math.h"
#include "OrthoCamera.h"

namespace PEANUT
{

    class Renderer2D
    {
    public:
        static void Init();
        static void BeginScene(const OrthoCamera& camera);
        static void DrawQuad(const glm::mat4 &transform, const glm::vec3 &color);
    };

} // namespace PEANUT
