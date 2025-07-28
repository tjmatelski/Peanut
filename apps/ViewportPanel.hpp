#pragma once

#include "Panel.hpp"
#include "peanut/Texture.hpp"

namespace PEANUT {
class Engine;

class ViewportPanel : public Panel {
public:
    ViewportPanel(class Engine* engine, const Texture* p_tex)
        : Panel("Viewport", engine)
        , p_tex_(p_tex)
    {
    }

    void Update() override;

    [[nodiscard]] float GetWidth() const { return m_width; }
    [[nodiscard]] float GetHeight() const { return m_height; }
    [[nodiscard]] bool IsFocused() const { return m_focused; }
    [[nodiscard]] bool IsHovered() const { return m_hovered; }

private:
    const Texture* p_tex_ = nullptr;
    float m_width = 0.0f;
    float m_height = 0.0f;
    bool m_focused = false;
    bool m_hovered = false;
};
}