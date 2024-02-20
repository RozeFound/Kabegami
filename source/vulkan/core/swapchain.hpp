#pragma once

#include "context.hpp"

namespace vki {

    struct Frame {

        uint32_t index;

        vk::Image image;
        vk::raii::ImageView view;

        std::unique_ptr<vk::raii::CommandBuffer> commands;
        std::unique_ptr<vk::raii::Framebuffer> buffer;

        vk::raii::Semaphore image_available;
        vk::raii::Semaphore render_finished;
        vk::raii::Fence in_flight;

    };

    class SwapChain {

        std::unique_ptr<vk::raii::SwapchainKHR> handle;
        std::unique_ptr<vk::raii::Queue> queue;
        const vk::raii::RenderPass& render_pass;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        std::vector<Frame> frames;
        vk::Extent2D extent;

        public:

        SwapChain (const vk::raii::RenderPass& render_pass) : render_pass(render_pass) { 
            auto present = context->get_queue_family_indices().present_family.value();
            queue = std::make_unique<vk::raii::Queue>(context->device, present, 0);
            create_handle(); make_frames();
        }

        void make_frames ();
        void create_handle ();

        uint32_t acquire_image (uint32_t index);
        bool present_image (uint32_t index);
        bool resize_if_needed();

        constexpr const auto& get_handle() const { return *handle; }
        constexpr const auto& operator* () const { return get_handle(); }

        constexpr const auto& get_extent() const { return extent; }
        constexpr auto& get_frames() { return frames; }

    };

}

