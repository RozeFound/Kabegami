#pragma once

#include <spdlog/spdlog.h>

#define logd(FS...) spdlog::get("kabegami")->debug(FS)
#define logv(FS...) spdlog::get("kabegami")->trace(FS)
#define logi(FS...) spdlog::get("kabegami")->info(FS)
#define logw(FS...) spdlog::get("kabegami")->warn(FS)
#define loge(FS...) spdlog::get("kabegami")->error(FS)

#if !defined(DNDEBUG)
    constexpr bool debug = true; 
#else
    constexpr bool debug = false;
#endif 

std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> make_debug_messenger (const vk::raii::Instance& instance);