#include "ViewportPanel.h"
#include "imgui.h"

namespace PEANUT {
void ViewportPanel::Update(const FrameBuffer& framebuffer)
{
    ImGui::Begin("Viewport");
    ImGui::GetWindowContentRegionMax();
    m_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    m_height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;
    ImGui::Image(reinterpret_cast<void*>(framebuffer.GetColorbufferTextureID()), { m_width, m_height });
    ImGui::End();
}
}