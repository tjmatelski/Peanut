#include <Renderer/OrthoCamera.h>

namespace PEANUT
{

    OrthoCamera::OrthoCamera() : m_position(0.0f, 0.0f, 0.0f), m_viewMatrix(1.0f),
        m_projectionMatrix(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f))
    {
        // Do not need to translate view matrix since position is 0, 0, 0
    }

    OrthoCamera::OrthoCamera(float left, float right, float bottom, float top) : m_position(0.0f, 0.0f, 0.0f),
        m_viewMatrix(1.0f), m_projectionMatrix(glm::ortho(left, right, bottom, top))
    {
        // Do not need to translate view matrix since position is 0, 0, 0
    }

    void OrthoCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_projectionMatrix = glm::ortho(left, right, bottom, top);
    }

    void OrthoCamera::SetPosition(float x, float y)
    {
        m_position = glm::vec3(x, y, 0.0f);
        FormViewMatrix();
    }

    void OrthoCamera::ZoomBy(const float zoom)
    {
        m_scaleFactor += zoom;
        if (m_scaleFactor <= 0.0f)
            m_scaleFactor = 0.0f;
        FormViewMatrix();
    }

    void OrthoCamera::FormViewMatrix()
    {
        m_viewMatrix = glm::translate(glm::mat4(1.0f), -m_position);
        m_viewMatrix = glm::scale(m_viewMatrix, glm::vec3(m_scaleFactor));
    }
}