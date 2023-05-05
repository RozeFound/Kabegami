#pragma once

#include "transient.hpp"

#include "vulkan/core/context.hpp"

namespace vku {

    struct opts {
        bool persistent = false;
        bool device_local = false;
    };

    template <opts options> class Buffer {

        std::unique_ptr<vk::raii::Buffer> handle;
        std::unique_ptr<vk::raii::DeviceMemory> memory;

        std::shared_ptr <vki::Context> context = vki::Context::get();

        void* data_location;
        std::size_t size;

        public:

        Buffer (std::size_t size, vk::BufferUsageFlags usage) : size(size) {

            auto flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            if constexpr (options.device_local) {
                usage |= vk::BufferUsageFlagBits::eTransferDst;
                flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
            }

            auto buffer_info = vk::BufferCreateInfo {
                .flags = vk::BufferCreateFlags(),
                .size = size, 
                .usage = usage,
                .sharingMode = vk::SharingMode::eExclusive
            };

            try { handle = std::make_unique<vk::raii::Buffer>(context->device, buffer_info); }
            catch (const vk::SystemError& e) { loge("Buffer creation failed: ", e.what()); }
            logi("Successfully created buffer"); 

            auto requirements = handle->getMemoryRequirements();

            auto allocation_info = vk::MemoryAllocateInfo {
                .allocationSize = requirements.size,
                .memoryTypeIndex = context->gpu.get_memory_index(requirements, flags)
            };

            try { memory = std::make_unique<vk::raii::DeviceMemory>(context->device, allocation_info); }
            catch (const vk::SystemError& e) { loge("Memory allocation failed: ", e.what()); }
            logi("Successfully allocated memory");

            handle->bindMemory(**memory, 0);

            if constexpr (options.persistent) data_location = memory->mapMemory(0, size);

        }

        ~Buffer () { if constexpr (options.persistent) memory->unmapMemory(); } 

        void upload (const auto& data, std::size_t size = 0, std::ptrdiff_t offset = 0) {

            if (!size) size = get_size();

            if constexpr (options.device_local) {

                auto staging = Buffer<opts{}>(size, vk::BufferUsageFlagBits::eTransferSrc);

                staging.upload(data, size, offset);

                TransientBuffer()->copyBuffer(**staging, **handle, vk::BufferCopy { .size = size });

            } else {

                void* location;
                if constexpr (options.persistent) location = data_location;
                else location = memory->mapMemory(0, size);
                
                location = static_cast<std::byte*>(location) + offset;
                std::memcpy(location, data, size);

                if constexpr (!options.persistent) memory->unmapMemory();

            }

        }

        constexpr const vk::raii::Buffer& operator*() const { return *handle; }
        constexpr const std::size_t get_size() const { return size; }

    };

    using BasicBuffer = Buffer<opts{}>;
    using DeviceBuffer = Buffer<opts{.device_local = true}>;
    using PersistentBuffer = Buffer<opts{.persistent = true}>;

}