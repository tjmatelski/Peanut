#include "RenderStatsPanel.hpp"
#include "Panel.hpp"

// external
#include <algorithm>
#include <imgui.h>

// stl
#include <chrono>
#include <iterator>

RenderStatsPanel::RenderStatsPanel(std::string_view name, PEANUT::Engine* p_engine)
    : Panel(name, p_engine)
    , m_avg_fps(60.0)
{
    std::ranges::fill(m_frame_times, 0.0f);
}

void RenderStatsPanel::Update()
{
    const auto current_time = std::chrono::high_resolution_clock::now();
    const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(current_time - m_last_frame_time);
    const auto delta_ns = static_cast<double>(delta.count());
    const auto fps = 1000000.0 / delta_ns;
    m_last_frame_time = current_time;

    // Calculate new moving averages
    constexpr double N = 30.0;
    m_avg_fps = m_avg_fps * (N - 1.0) / N + fps / N;
    const double avg_frame_time = m_frame_times.back() * (N - 1.0) / N + (delta_ns / 1000.0) / N;

    // Append frame time to end of list
    std::rotate(m_frame_times.begin(), m_frame_times.end(), std::next(m_frame_times.begin()));
    m_frame_times.back() = avg_frame_time;

    ImGui::Text("FPS: %.0f", m_avg_fps);
    ImGui::Text("Current Frame Time: %g ms", m_frame_times.back());
    ImGui::PlotLines("Frame Times", m_frame_times.data(), m_frame_times.size());
}