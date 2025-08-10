#pragma once

#include <peanut/Engine.hpp>

// external
#include <imgui.h>

// stl
#include <string>
#include <string_view>

class Panel {
public:
    Panel(std::string_view name, PEANUT::Engine* p_engine)
        : m_name(name)
        , m_engine(p_engine)
    {
    }

    virtual ~Panel() = default;

    void Begin() { ImGui::Begin(m_name.c_str()); }

    virtual void Update() { }

    void End() { ImGui::End(); }

    auto Engine() -> PEANUT::Engine* { return m_engine; }
    auto Name() const -> const std::string& { return m_name; }

private:
    std::string m_name;
    PEANUT::Engine* m_engine = nullptr;
};
