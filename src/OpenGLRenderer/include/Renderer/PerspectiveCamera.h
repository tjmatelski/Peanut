#pragma once

#include "Utils/Math.h"

namespace PEANUT {

class PerspectiveCamera {
public:
    PerspectiveCamera(const glm::vec3& position);
    ~PerspectiveCamera() = default;

    glm::mat4 GetViewMatrix() const;
    inline const glm::vec3& GetFront() const { return m_front; }
    inline const glm::vec3& GetPosition() const { return m_position; }
    inline const glm::mat4& GetProjectionMatrix() const { return m_projection; }

    void SetAspectRatio(float width, float height);
    void SetAspectRatio(int width, int height);

    void MoveForward(float timeStep);
    void MoveBackward(float timeStep);
    void MoveLeft(float timeStep);
    void MoveRight(float timeStep);
    void MoveUp(float timeStep);
    void MoveDown(float timeStep);
    void PitchBy(float value);
    void YawBy(float value);

private:
    static constexpr float c_speed = 3.0f;
    static constexpr glm::vec3 c_worldUp = { 0.0, 1.0, 0.0 };
    glm::vec3 m_position;
    glm::vec3 m_front = { 0.0, 0.0, -1.0 };
    glm::vec3 m_right = { 1.0, 0.0, 0.0 };
    glm::vec3 m_up = { 0.0, 1.0, 0.0 };
    float m_pitch;
    float m_yaw;
    float m_fov;
    float m_aspectRatio;
    float m_nearCutoff;
    float m_farCutoff;
    glm::mat4 m_projection;

    void UpdateVectors();
    void UpdateProjection();
};

}
