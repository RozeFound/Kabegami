#include "device.hpp"

#include <set>

namespace vki {

    Device::Device (const vk::raii::PhysicalDevice& gpu, vku::QueueFamilyIndices indices) {

        auto queue_info = std::vector<vk::DeviceQueueCreateInfo>();
        auto queue_piority = 1.f;

        auto unique_indices = std::set {
            indices.transfer_family.value(),
            indices.graphics_family.value(), 
            indices.present_family.value()
        };

        for (const auto& queue_family : unique_indices) {

            auto create_info = vk::DeviceQueueCreateInfo {
                .flags = vk::DeviceQueueCreateFlags(),
                .queueFamilyIndex = queue_family,
                .queueCount = 1,
                .pQueuePriorities = &queue_piority
            }; queue_info.push_back(create_info);

        }

        auto device_features = vk::PhysicalDeviceFeatures();
        device_features.samplerAnisotropy = VK_TRUE;
        device_features.sampleRateShading = VK_TRUE;

        auto extensions = get_extensions();
        auto layers = get_layers();

        auto device_info = vk::DeviceCreateInfo {
            .flags = vk::DeviceCreateFlags(),
            .queueCreateInfoCount = vku::to_u32(queue_info.size()),
            .pQueueCreateInfos = queue_info.data(),
            .enabledLayerCount = vku::to_u32(layers.size()),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = vku::to_u32(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = &device_features
        };

        try { handle = std::make_unique<vk::raii::Device>(gpu, device_info); } 
        catch (vk::SystemError e) { loge("Failed to abstract Physical Device: {}", e.what()); }

    }

    std::vector<const char*> Device::get_extensions() {

        return std::vector<const char*>{"VK_KHR_swapchain"};

    }

    std::vector<const char*> Device::get_layers() {

        auto layers = std::vector<const char*>();

        if constexpr (debug) layers.push_back("VK_LAYER_KHRONOS_validation");

        return layers;

    }

}