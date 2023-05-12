#pragma once

#include "transient.hpp"

#include "vulkan/core/context.hpp"

namespace vku {

    struct Opts {
        bool persistent = false;
        bool device_local = false;
    };

    template <Opts opts> class Buffer {

        std::unique_ptr<vk::raii::Buffer> handle;
        std::unique_ptr<vk::raii::DeviceMemory> memory;

        std::shared_ptr <vki::Context> context = vki::Context::get();

        void* data_location;
        std::size_t size;

        public:

        Buffer (std::size_t size, vk::BufferUsageFlags usage) : size(size) {

            auto flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

            if constexpr (opts.device_local) {
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
            catch (const vk::SystemError& e) { loge("Buffer creation failed: {}", e.what()); }

            auto requirements = handle->getMemoryRequirements();

            auto allocation_info = vk::MemoryAllocateInfo {
                .allocationSize = requirements.size,
                .memoryTypeIndex = context->gpu.get_memory_index(requirements, flags)
            };

            try { memory = std::make_unique<vk::raii::DeviceMemory>(context->device, allocation_info); }
            catch (const vk::SystemError& e) { loge("Memory allocation failed: {}", e.what()); }

            handle->bindMemory(**memory, 0);

            if constexpr (opts.persistent) data_location = memory->mapMemory(0, size);

        }

        ~Buffer () { if constexpr (opts.persistent) memory->unmapMemory(); } 

        void upload (const auto& data, std::size_t size = 0, std::ptrdiff_t offset = 0) {

            if (!size) size = get_size();

            if constexpr (opts.device_local) {

                auto staging = Buffer<Opts{}>(size, vk::BufferUsageFlagBits::eTransferSrc);

                staging.upload(data, size, offset);

                TransientBuffer()->copyBuffer(**staging, **handle, vk::BufferCopy { .size = size });

            } else {

                void* location;
                if constexpr (opts.persistent) location = data_location;
                else location = memory->mapMemory(0, size);
                
                location = static_cast<std::byte*>(location) + offset;
                std::memcpy(location, data, size);

                if constexpr (!opts.persistent) memory->unmapMemory();

            }

        }

        constexpr const vk::raii::Buffer& operator*() const { return *handle; }
        constexpr const std::size_t get_size() const { return size; }

    };

    using BasicBuffer = Buffer<Opts{}>;
    using DeviceBuffer = Buffer<Opts{.device_local = true}>;
    using PersistentBuffer = Buffer<Opts{.persistent = true}>;

}