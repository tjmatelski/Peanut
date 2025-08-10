#pragma once

// external

namespace PEANUT {
struct DebugConfig {
    enum class View { None, Shadow };
    enum class LogLevel { Error, Warn, Info, Debug, Trace };

    LogLevel log_level_ = LogLevel::Info;
    View debug_view_ = View::None;
};
}
