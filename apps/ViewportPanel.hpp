#pragma once

#include "Panel.hpp"
#include <peanut/FrameBuffer.hpp>

namespace PEANUT {
class Engine;

class ViewportPanel : public Panel {
public:
    ViewportPanel(class Engine* engine, const FrameBuffer* fb)
        : Panel("Viewport", engine)
        , m_framebuffer(fb)
    {
    }

    void Update() override;

    [[nodiscard]] float GetWidth() const { return m_width; }
    [[nodiscard]] float GetHeight() const { return m_height; }
    [[nodiscard]] bool IsFocused() const { return m_focused; }
    [[nodiscard]] bool IsHovered() const { return m_hovered; }

private:
    const FrameBuffer* m_framebuffer = nullptr;
    float m_width = 0.0f;
    float m_height = 0.0f;
    bool m_focused = false;
    bool m_hovered = false;
};
}