#pragma once

#include "window.hpp"

#include "vulkan/instance.hpp"
#include "vulkan/surface.hpp"
#include "vulkan/device.hpp"
#include "vulkan/gpu.hpp"

#include "vulkan/utils.hpp"

namespace vki {

    class Context {

        vk::raii::Context handle;

        const Window& window;
        const Instance instance;
        const Surface surface;

        const GPU gpu;
        const Device device;
        

        public:

        Context (const Window& window) : window(window), instance(handle), 
            surface(*instance, *window), gpu(*instance),  device(*gpu, get_queue_family_indices()) { }

        constexpr const auto& get_handle() const { return handle; }

        constexpr const Instance& get_instance() const { return instance; }
        constexpr const Window& get_window() const { return window; }

        const vku::QueueFamilyIndices get_queue_family_indices() const;
    };

}