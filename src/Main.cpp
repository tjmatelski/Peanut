// peanut
#include "EngineImpl.hpp"
#include "Settings.hpp"
#include "peanut/Application.hpp"
#include "peanut/Engine.hpp"
#include "peanut/Log.hpp"

// external
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>

// stl
#include <memory>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto basic_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("peanut.log", true);
    std::vector<spdlog::sink_ptr> sinks { console_sink, basic_sink };
    auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(), sinks.end());
    spdlog::register_logger(logger); // if it would be used in some other place
    spdlog::set_default_logger(logger);
    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

    LOG_INFO("Starting Application: {}", argv[0]);
    PEANUT::Settings::SetApplication(argv[0]);

    PEANUT::Engine engine;
    auto& engine_impl = PEANUT::EngineImpl::Get();
    engine_impl.m_app = PEANUT::GetApplication();
    engine.m_engine = &engine_impl;
    engine_impl.m_app->m_engine = &engine;
    engine_impl.Run();

    return 0;
}