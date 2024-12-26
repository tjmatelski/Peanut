#include "ViewportPanel.hpp"

// external
#include "imgui.h"

namespace PEANUT {
void ViewportPanel::Update()
{
    m_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    m_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
    m_focused = ImGui::IsWindowFocused();
    m_hovered = ImGui::IsWindowHovered();
    ImGui::Image(reinterpret_cast<void*>(m_framebuffer->GetColorbufferTextureID()), { m_width, m_height }, { 0, 1 }, { 1, 0 });
}
}