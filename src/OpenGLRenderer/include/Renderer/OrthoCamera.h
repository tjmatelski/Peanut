#pragma once

#include "Utils/Math.h"

namespace PEANUT {

class OrthoCamera {
public:
    OrthoCamera();
    OrthoCamera(float left, float right, float bottom, float top);

    void SetProjection(float left, float right, float bottom, float top);
    void SetPosition(float x, float y);
    void ZoomBy(const float zoom);

    inline const glm::vec3& GetPosition() const { return m_position; }
    inline const glm::mat4& GetViewMatrix() const { return m_viewMatrix; }
    inline const glm::mat4& GetProjectionMatrix() const { return m_projectionMatrix; }

private:
    glm::vec3 m_position;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    float m_scaleFactor = 1.0f;

    void FormViewMatrix();
};

}