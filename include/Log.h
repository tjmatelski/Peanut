#pragma once

#include <spdlog/spdlog.h>

#define LOG_ERROR(content) spdlog::error(content)
#define LOG_INFO(content) spdlog::info(content)