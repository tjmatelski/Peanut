#include "DebugPanel.hpp"

// external
#include "imgui.h"

namespace PEANUT {
void DebugPanel::Update()
{
    ImGui::BeginCombo("Log Level", "Idk");
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