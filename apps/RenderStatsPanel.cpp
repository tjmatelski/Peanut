#include "RenderStatsPanel.hpp"

#include <imgui.h>

#include <chrono>

void RenderStatsPanel::Update()
{
    const auto current_time = std::chrono::high_resolution_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(current_time - m_last_frame_time);
    const auto delta_ns = static_cast<double>(delta.count());
    const auto fps = 1000000.0 / delta_ns;
    m_last_frame_time = current_time;

    ImGui::Begin("Render Stats");
    ImGui::Text("FPS: %.0f", fps);
    ImGui::Text("Frame Time: %g ms", delta_ns / 1000.0);
    ImGui::End();
}