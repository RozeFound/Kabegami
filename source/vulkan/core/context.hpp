#pragma once

#include "window.hpp"

#include "instance.hpp"
#include "surface.hpp"
#include "device.hpp"
#include "gpu.hpp"

#include "vulkan/utils.hpp"

namespace vki {

    struct Context {

        const vk::raii::Context handle;

        public:

        const Window& window;
        const Instance instance;
        const Surface surface;

        const GPU gpu;
        const Device device;

        Context (const Window& window) : window(window), instance(handle), 
            surface(*instance, *window), gpu(*instance), device(*gpu, get_queue_family_indices()) {}

        static void set (std::shared_ptr<Context>);
        static const std::shared_ptr<Context> get();

        constexpr const auto& get_handle() const { return handle; }

        const vku::QueueFamilyIndices get_queue_family_indices() const;

        const vk::Extent2D get_extent() const;
        const vk::SurfaceFormatKHR get_format() const;
        
    };

}