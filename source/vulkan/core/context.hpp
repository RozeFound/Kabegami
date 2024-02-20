#pragma once

#include "window.hpp"

#include "helpers/instance.hpp"
#include "helpers/surface.hpp"
#include "helpers/device.hpp"
#include "helpers/gpu.hpp"

#include "vulkan/utility/misc.hpp"

namespace vki {

    struct Context {

        const vk::raii::Context handle;

        const Window& window;
        const Instance instance;
        const Surface surface;

        const GPU gpu;
        const Device device;

        Context (const Window& window) 
            : window(window), instance(handle), 
            surface(*instance, *window), gpu(*instance), 
            device(*gpu, get_queue_family_indices()) {}

        static void set (std::shared_ptr<Context>);
        static const std::shared_ptr<Context> get();

        const vk::Extent2D get_extent() const;
        const vk::SurfaceFormatKHR get_format() const;
        const vku::Version get_version() const;

        const vku::QueueFamilyIndices get_queue_family_indices() const;
        std::unique_ptr<vk::raii::RenderPass> create_render_pass();
        
    };

}