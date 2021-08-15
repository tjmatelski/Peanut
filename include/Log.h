#pragma once

#include <spdlog/spdlog.h>

#define LOG_ERROR(content, ...) spdlog::error(content, ##__VA_ARGS__)
#define LOG_INFO(content, ...) spdlog::info(content, ##__VA_ARGS__)
#define LOG_DEBUG(content, ...) spdlog::debug(content, ##__VA_ARGS__)