#pragma once

#include "core/context.hpp"

namespace vki {

    struct Frame {

        uint32_t index;

        vk::Image image;
        std::unique_ptr <vk::raii::ImageView> view;

        std::unique_ptr <vk::raii::CommandBuffer> commands;
        std::unique_ptr <vk::raii::Framebuffer> buffer;

        std::unique_ptr <vk::raii::Semaphore> image_available;
        std::unique_ptr <vk::raii::Semaphore> render_finished;
        std::unique_ptr <vk::raii::Fence> in_flight;

    };

    class SwapChain {

        std::unique_ptr <vk::raii::SwapchainKHR> handle;
        std::unique_ptr <vk::raii::Queue> queue;

        std::shared_ptr <vki::Context> context = vki::Context::get();

        std::vector<Frame> frames;
        vk::Extent2D extent;

        public:

        SwapChain(const vk::raii::RenderPass& render_pass) { 
            auto graphics = context->get_queue_family_indices().graphics_family.value();
            queue = std::make_unique<vk::raii::Queue>(context->device, graphics, 0);
            create_handle(); make_frames(render_pass);
        }

        void make_frames (const vk::raii::RenderPass&);
        void create_handle (bool vsync = false);

        uint32_t acquire_image (uint32_t index);
        bool present_image (uint32_t index);
        bool resize_if_needed();

        constexpr const auto& get_handle() const { return *handle; }
        constexpr const auto& operator* () const { return get_handle(); }

        constexpr const auto& get_frames() const { return frames; }
        constexpr const auto& get_extent() const { return extent; }

    };

}

