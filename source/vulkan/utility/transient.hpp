#pragma once

#include "vulkan/core/context.hpp"

namespace vku {

    class TransientBuffer {

        std::unique_ptr <vk::raii::Queue> queue;
        std::unique_ptr <vk::raii::CommandPool> pool;
        std::unique_ptr <vk::raii::CommandBuffer> buffer;
        std::unique_ptr <vk::raii::Fence> submition_completed;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        public:

        TransientBuffer (bool graphics_capable = false);
        ~TransientBuffer();

        constexpr const auto& operator->() const { return buffer; }

    };

}
