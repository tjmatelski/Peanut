#pragma once

#include <peanut/Material.hpp>
#include <peanut/Math.hpp>
#include <peanut/Mesh.hpp>

// stl
#include <filesystem>
#include <string>

namespace PEANUT {

class PythonScript;

struct TagComponent {
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
        return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(glm::quat(rotation))
            * glm::scale(glm::mat4(1.0f), scale);
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

struct PythonScriptComponent {
    std::filesystem::path script;
    PythonScript* script_obj = nullptr;
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

struct Renderable {
    Mesh mesh_;
    Material material_;
    const Shader* shader_;
};

}
