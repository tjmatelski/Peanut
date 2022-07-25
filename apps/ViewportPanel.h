#pragma once

#include <Peanut.h>

namespace PEANUT {
class ViewportPanel {
public:
    void Update(const FrameBuffer& framebuffer);
    [[nodiscard]] float GetWidth() const { return m_width; }
    [[nodiscard]] float GetHeight() const { return m_height; }

private:
    float m_width = 0.0f;
    float m_height = 0.0f;
};
}