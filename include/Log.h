#pragma once

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif
#include <spdlog/spdlog.h>

#define LOG_ERROR(content, ...) SPDLOG_ERROR(content, ##__VA_ARGS__)
#define LOG_WARN(content, ...) SPDLOG_WARN(content, ##__VA_ARGS__)
#define LOG_INFO(content, ...) SPDLOG_INFO(content, ##__VA_ARGS__)
#define LOG_DEBUG(content, ...) SPDLOG_DEBUG(content, ##__VA_ARGS__)
#define LOG_TRACE(content, ...) SPDLOG_TRACE(content, ##__VA_ARGS__)