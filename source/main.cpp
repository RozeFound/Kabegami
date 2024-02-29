#include <vector>
#include <string_view>
#include <memory>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "app.hpp"

auto main (const int argc, const char* const* const argv) -> int {

    const auto args = std::vector<std::string_view>(argv, argv + argc);
    
    spdlog::init_thread_pool(8192, 1);
    std::vector<spdlog::sink_ptr> sinks;

    if constexpr (debug) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        sinks.push_back(console_sink);
    };

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("kabegami.log",  true);
    file_sink->set_level(spdlog::level::trace);
    sinks.push_back(file_sink);

    auto logger = std::make_shared<spdlog::async_logger>("kabegami", sinks.begin(), sinks.end(),
                                   spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    logger->set_level(spdlog::level::trace);

    spdlog::register_logger(logger);

    auto app = std::make_unique<Kabegami>();

    app->run();

    return 0;

}