#pragma once

#include "Utils/Math.h"
#include "Utils/TimeStep.h"

#include <filesystem>
#include <memory>
#include <string>

namespace PEANUT {

struct TagComponent {
    TagComponent() = default;
    std::string tag;
};

struct TransformComponent {
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

struct SpriteRenderComponent {
    SpriteRenderComponent() = default;
    SpriteRenderComponent(float r, float g, float b)
        : color(r, g, b)
    {
    }
    SpriteRenderComponent(const glm::vec3& rgb)
        : color(rgb)
    {
    }

    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
    std::string texture = {};
};

struct LuaScriptComponent {
    std::filesystem::path script;
};

struct ModelFileComponent {
    std::filesystem::path file;
};

struct DirectionalLightComponent {
    glm::vec3 direction = { 0.0f, -1.0f, 0.0f };
    float ambient = 0.05f;
    float diffuse = 0.4f;
    float specular = 0.5f;
};

struct PointLightComponent {
    glm::vec3 color = { 1.0f, 1.0f, 1.0f };
    float ambient = 0.05f;
    float diffuse = 0.8f;
    float specular = 1.0f;
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SkyboxComponent {
    std::filesystem::path directory;
};

}
