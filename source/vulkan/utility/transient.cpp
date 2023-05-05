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
    
}