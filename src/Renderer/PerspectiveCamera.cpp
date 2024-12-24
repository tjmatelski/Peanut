#include <peanut/PerspectiveCamera.hpp>

namespace PEANUT {
PerspectiveCamera::PerspectiveCamera(const glm::vec3& position)
    : m_position(position)
    , m_pitch(0.0f)
    , m_yaw(90.0)
    , m_fov(45.0)
    , m_aspectRatio(16.0 / 9.0)
    , m_nearCutoff(0.1)
    , m_farCutoff(100.0)
{
    UpdateVectors();
    UpdateProjection();
}

glm::mat4 PerspectiveCamera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void PerspectiveCamera::SetAspectRatio(float width, float height)
{
    m_aspectRatio = width / height;
    UpdateProjection();
}

void PerspectiveCamera::SetAspectRatio(int width, int height)
{
    SetAspectRatio(static_cast<float>(width), static_cast<float>(height));
}

void PerspectiveCamera::MoveForward(float timeStep)
{
    m_position += c_speed * m_front * timeStep;
}

void PerspectiveCamera::MoveBackward(float timeStep)
{
    m_position -= c_speed * m_front * timeStep;
}

void PerspectiveCamera::MoveLeft(float timeStep)
{
    m_position -= m_right * c_speed * timeStep;
}

void PerspectiveCamera::MoveRight(float timeStep)
{
    m_position += m_right * c_speed * timeStep;
}

void PerspectiveCamera::MoveUp(float timeStep)
{
    m_position += m_up * c_speed * timeStep;
}

void PerspectiveCamera::MoveDown(float timeStep)
{
    m_position -= m_up * c_speed * timeStep;
}

void PerspectiveCamera::PitchBy(float value)
{
    m_pitch += value;
    m_pitch = glm::clamp(m_pitch, -89.9f, 89.9f);
    UpdateVectors();
}

void PerspectiveCamera::YawBy(float value)
{
    m_yaw += value;
    UpdateVectors();
}

void PerspectiveCamera::UpdateVectors()
{
    m_front.x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front.y = glm::sin(glm::radians(m_pitch));
    m_front.z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);
    m_right = glm::normalize(glm::cross(m_front, c_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void PerspectiveCamera::UpdateProjection()
{
    m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearCutoff, m_farCutoff);
}
}
