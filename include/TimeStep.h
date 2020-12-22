#pragma once

namespace PEANUT
{

    class TimeStep
    {
    public:
        TimeStep(const float time) : m_time(time) {}
        operator float() const { return m_time; }
        inline float GetTime() const { return m_time; }

    private:
        float m_time;
    };
} // namespace PEANUT