#pragma once

#include "Log.hpp"

namespace PEANUT {
struct DebugConfig {
    enum class View { None, Shadow };

    spdlog::level::level_enum log_level_ = spdlog::level::level_enum::info;
    View debug_view_ = View::None;
};
}
