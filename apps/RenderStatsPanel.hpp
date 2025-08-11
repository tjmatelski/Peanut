#pragma once

#include "Panel.hpp"

// stl
#include <array>
#include <chrono>

class RenderStatsPanel : public Panel {
public:
    RenderStatsPanel(PEANUT::Engine* p_engine);
    void Update() override;

private:
    std::array<float, 256> m_frame_times;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_frame_time;
    double m_avg_fps;
};
