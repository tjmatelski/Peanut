#pragma once

#include <peanut/FrameBuffer.hpp>

namespace PEANUT {
class ViewportPanel {
public:
    void Update(const FrameBuffer& framebuffer);
    [[nodiscard]] float GetWidth() const { return m_width; }
    [[nodiscard]] float GetHeight() const { return m_height; }
    [[nodiscard]] bool IsFocused() const { return m_focused; }
    [[nodiscard]] bool IsHovered() const { return m_hovered; }

private:
    float m_width = 0.0f;
    float m_height = 0.0f;
    bool m_focused = false;
    bool m_hovered = false;
};
}