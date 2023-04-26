#pragma once

#include <spdlog/spdlog.h>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <spdlog/sinks/stdout_color_sinks.h>


#define logv(FS...) spdlog::get("kabegami")->trace(FS)
#define logi(FS...) spdlog::get("kabegami")->info(FS)
#define logw(FS...) spdlog::get("kabegami")->warn(FS)
#define loge(FS...) spdlog::get("kabegami")->error(FS)

#if defined(DEBUG)
    constexpr bool debug = true; 
#else
    constexpr bool debug = false;
#endif 

inline void setup_logger() {

    spdlog::init_thread_pool(8192, 1);
    std::vector<spdlog::sink_ptr> sinks;

    if constexpr (debug) sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("kabegami.log"));

    auto logger = std::make_shared<spdlog::async_logger>("kabegami", sinks.begin(), sinks.end(),
                                    spdlog::thread_pool(), spdlog::async_overflow_policy::block);

    spdlog::register_logger(logger);

}