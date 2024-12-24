#pragma once

// stl
#include <chrono>

class RenderStatsPanel {
public:
    void Update();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_frame_time;
};
