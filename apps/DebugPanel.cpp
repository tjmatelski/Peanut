#include "DebugPanel.hpp"

// external
#include "imgui.h"
#include "peanut/DebugConfig.hpp"

namespace PEANUT {
void DebugPanel::Update()
{
    // log level
    constexpr auto log_name = [](DebugConfig::LogLevel log_level) {
        switch (log_level) {
        case DebugConfig::LogLevel::Error:
            return "Error";
        case DebugConfig::LogLevel::Warn:
            return "Warning";
        case DebugConfig::LogLevel::Info:
            return "Info";
        case DebugConfig::LogLevel::Debug:
            return "Debug";
        case DebugConfig::LogLevel::Trace:
            return "Trace";
        }
        return "shouldn't see this";
    };
    if (ImGui::BeginCombo("Log Level", log_name(Engine()->GetDebugConfig().log_level_))) {
        if (ImGui::Selectable("Warn")) {
            Engine()->GetDebugConfig().log_level_ = DebugConfig::LogLevel::Warn;
        }
        if (ImGui::Selectable("Info")) {
            Engine()->GetDebugConfig().log_level_ = DebugConfig::LogLevel::Info;
        }
        if (ImGui::Selectable("Debug")) {
            Engine()->GetDebugConfig().log_level_ = DebugConfig::LogLevel::Debug;
        }
        if (ImGui::Selectable("Trace")) {
            Engine()->GetDebugConfig().log_level_ = DebugConfig::LogLevel::Trace;
        }
        ImGui::EndCombo();
    }

    // Debug view (renders in the bottom corner)
    constexpr auto debug_view_name = [](DebugConfig::View debug_view) {
        switch (debug_view) {
        case DebugConfig::View::None:
            return "None";
        case DebugConfig::View::Shadow:
            return "Shadow Map Buffer";
        }
        return "shouldn't see this";
    };
    if (ImGui::BeginCombo("Debug View", debug_view_name(Engine()->GetDebugConfig().debug_view_))) {
        if (ImGui::Selectable("None")) {
            Engine()->GetDebugConfig().debug_view_ = DebugConfig::View::None;
        }
        if (ImGui::Selectable("Shadow Map Buffer")) {
            Engine()->GetDebugConfig().debug_view_ = DebugConfig::View::Shadow;
        }
        ImGui::EndCombo();
    }
}
}