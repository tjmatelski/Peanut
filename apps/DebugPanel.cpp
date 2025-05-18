#include "DebugPanel.hpp"

// external
#include "imgui.h"
#include <spdlog/common.h>

namespace PEANUT {
void DebugPanel::Update()
{
    constexpr auto log_name = [](spdlog::level::level_enum log_level) {
        switch (log_level) {
        case spdlog::level::level_enum::off:
            return "Off";
        case spdlog::level::level_enum::critical:
            return "Critical";
        case spdlog::level::level_enum::err:
            return "Error";
        case spdlog::level::level_enum::warn:
            return "Warning";
        case spdlog::level::level_enum::info:
            return "Info";
        case spdlog::level::level_enum::debug:
            return "Debug";
        case spdlog::level::level_enum::trace:
            return "Trace";
        case spdlog::level::level_enum::n_levels:
            return "shouldn't see this";
        }
        return "shouldn't see this";
    };
    if (ImGui::BeginCombo("Log Level", log_name(Engine()->GetDebugConfig().log_level_))) {
        if (ImGui::Selectable("Warn")) {
            Engine()->GetDebugConfig().log_level_ = spdlog::level::level_enum::warn;
        }
        if (ImGui::Selectable("Info")) {
            Engine()->GetDebugConfig().log_level_ = spdlog::level::level_enum::info;
        }
        if (ImGui::Selectable("Debug")) {
            Engine()->GetDebugConfig().log_level_ = spdlog::level::level_enum::debug;
        }
        if (ImGui::Selectable("Trace")) {
            Engine()->GetDebugConfig().log_level_ = spdlog::level::level_enum::trace;
        }
        ImGui::EndCombo();
    }
}
}