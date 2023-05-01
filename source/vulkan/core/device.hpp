#pragma once

#include "vulkan/utils.hpp"

namespace vki {

    class Device {

        std::unique_ptr <vk::raii::Device> handle;

        std::vector <const char*> get_extensions();
        std::vector <const char*> get_layers();

        public:

        Device (const vk::raii::PhysicalDevice& gpu, vku::QueueFamilyIndices indices);

        constexpr const auto& get_handle() const { return *handle; }
        constexpr const auto& operator* () const { return get_handle(); }
        constexpr const auto& operator->() const { return handle; }

    };

}