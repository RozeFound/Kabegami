#include "transient.hpp"

namespace vku {

    TransientBuffer::TransientBuffer (bool graphics_capable) {

        auto indices = context->get_queue_family_indices();
        auto queue_family_index = graphics_capable ? indices.graphics_family.value() : indices.transfer_family.value();
        queue = std::make_unique<vk::raii::Queue>(context->device, queue_family_index, 0);

        auto create_info = vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eTransient,
            .queueFamilyIndex = queue_family_index
        };

        pool = std::make_unique<vk::raii::CommandPool>(context->device, create_info);

        auto allocate_info = vk::CommandBufferAllocateInfo {
            .commandPool = **pool,
            .level = vk::CommandBufferLevel::ePrimary,
            .commandBufferCount = 1,
        };

        auto buffers = vk::raii::CommandBuffers(context->device, allocate_info);

        buffer = std::make_unique<vk::raii::CommandBuffer>(std::move(buffers.at(0)));
        auto fence_info = vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled };
        submition_completed = std::make_unique<vk::raii::Fence>(context->device, fence_info);

        auto begin_info = vk::CommandBufferBeginInfo {
            .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
        };

        buffer->begin(begin_info);

    }
    
    TransientBuffer::~TransientBuffer() {

        buffer->end();

        auto submit_info = vk::SubmitInfo {
            .commandBufferCount = 1,
            .pCommandBuffers = &**buffer
        };

        context->device->resetFences(**submition_completed);
        queue->submit(submit_info, **submition_completed);

        constexpr auto timeout = std::numeric_limits<uint64_t>::max();
        auto wait =context->device->waitForFences(**submition_completed, true, timeout);
        if (wait != vk::Result::eSuccess) logw("Something goes wrong when waiting on fences");

    }

    void TransientBuffer::barrier (const vk::raii::Image& image,
        const vk::ImageAspectFlags aspect_flags, 
        const std::array<vk::PipelineStageFlags, 2> stages, 
        const std::array<vk::AccessFlags, 2> access_flags, 
        const std::array<vk::ImageLayout, 2> layouts, uint32_t mip_levels) const {

        auto subres_range = vk::ImageSubresourceRange {
            .aspectMask = aspect_flags,
            .baseMipLevel = 0,
            .levelCount = mip_levels,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        auto barrier = vk::ImageMemoryBarrier {
            .srcAccessMask = access_flags.at(0),
            .dstAccessMask = access_flags.at(1),
            .oldLayout = layouts.at(0),
            .newLayout = layouts.at(1),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = *image,
            .subresourceRange = subres_range
        };

        return this->barrier(barrier, stages);

    }
    
}