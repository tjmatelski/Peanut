#pragma once

#include <peanut/Math.hpp>

namespace PEANUT {

struct DirectionalLight {
    glm::vec3 direction = { 0.0f, -1.0f, 0.0f };
    glm::vec3 ambient = { 0.05, 0.05, 0.05 };
    glm::vec3 diffuse = { 0.4, 0.4, 0.4 };
    glm::vec3 specular = { 0.5, 0.5, 0.5 };
};

struct PointLight {
    bool active = true;
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 ambient = { 0.05f, 0.05f, 0.05f };
    glm::vec3 diffuse = { 0.8f, 0.8f, 0.8f };
    glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight {
    bool active = true;
    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 direction = { 0.0f, 0.0f, 1.0f };
    glm::vec3 diffuse = { 0.5f, 0.5f, 0.5f };
    glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
    float beginCutoff = 5.0f; ///< In degrees
    float endCutoff = 12.0f; ///< In degrees
};

}
