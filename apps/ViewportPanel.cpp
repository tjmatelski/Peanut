#include "Peanut.h"
#include "Renderer/FrameBuffer.h"

namespace {
void UpdateViewportPanelImpl(const PEANUT::FrameBuffer& framebuffer)
{
    ImGui::Begin("Viewport");
    ImGui::Image(reinterpret_cast<void*>(framebuffer.GetColorbufferTextureID()), { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() });
    ImGui::End();
}
}

namespace PEANUT {
void UpdateViewportPanel(const FrameBuffer& framebuffer)
{
    UpdateViewportPanelImpl(framebuffer);
}
}