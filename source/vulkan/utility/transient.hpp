#pragma once

#include "vulkan/core/context.hpp"

namespace vku {

    class TransientBuffer {

        std::unique_ptr<vk::raii::Queue> queue;
        std::unique_ptr<vk::raii::CommandPool> pool;
        std::unique_ptr<vk::raii::CommandBuffer> buffer;
        std::unique_ptr<vk::raii::Fence> submition_completed;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        public:

        TransientBuffer (bool graphics_capable = false);
        ~TransientBuffer();

        constexpr void barrier (const vk::ImageMemoryBarrier& barrier, const std::array<vk::PipelineStageFlags, 2> stages) {
            buffer->pipelineBarrier(stages.at(0), stages.at(1), vk::DependencyFlags(), nullptr, nullptr, barrier);
        }

        void barrier (const vk::raii::Image& image, const vk::ImageAspectFlags aspect_flags, const std::array<vk::PipelineStageFlags, 2> stages, 
            const std::array<vk::AccessFlags, 2> access_flags, const std::array<vk::ImageLayout, 2> layouts, uint32_t mip_levels = 1);

        constexpr const auto& operator->() const { return buffer; }

    };

}
