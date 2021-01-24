#pragma once

#include "../Math.h"
#include <string>

namespace PEANUT
{

struct TagComponent
{
    TagComponent() = default;
    std::string tag;
};

struct TransformComponent
{
    TransformComponent() = default;

    glm::vec3 translation;
    glm::vec3 rotation; ///< In radians
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    operator glm::mat4() const { return GetTransform(); }

    glm::mat4 GetTransform() const
    {
        return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
    }
};

struct SpriteRenderComponent
{
    SpriteRenderComponent() = default;
    SpriteRenderComponent(float r, float g, float b) : color(r, g, b) {}
    SpriteRenderComponent(const glm::vec3& rgb) : color(rgb) {}

    glm::vec3 color = {1.0f, 1.0f, 1.0f};
    std::string texture = "./res/textures/BlankSquare.png";
};
}