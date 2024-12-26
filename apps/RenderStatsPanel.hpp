#pragma once

#include "Panel.hpp"

// stl
#include <chrono>

class RenderStatsPanel : public Panel {
public:
    using Panel::Panel;
    void Update() override;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_frame_time;
};
