#include "gpu.hpp"

namespace vki {

    GPU::GPU (const vk::raii::Instance& instance) {

        auto suitable = [] (vk::raii::PhysicalDevice& device) {

            for (auto& properties : device.enumerateDeviceExtensionProperties())
                if (!std::strcmp(properties.extensionName, "VK_KHR_swapchain"))
                    return true;

            return false;

        };

        auto devices = vk::raii::PhysicalDevices(instance);
        auto device = std::ranges::find_if(devices, suitable);
        if (device == std::end(devices)) loge("Failed to get Physical Device");

        handle = std::make_unique<vk::raii::PhysicalDevice>(*device);
        log_properties();

    }

    void GPU::log_properties() {

        auto properties = handle->getProperties();

        std::string_view device_type;

        switch (properties.deviceType) {
            case (vk::PhysicalDeviceType::eCpu): 
                device_type = "CPU"; break;
            case (vk::PhysicalDeviceType::eDiscreteGpu): 
                device_type = "Discrete GPU"; break;
            case (vk::PhysicalDeviceType::eIntegratedGpu): 
                device_type = "Integrated GPU"; break;
            case (vk::PhysicalDeviceType::eVirtualGpu): 
                device_type = "Virtual GPU"; break;
            default: device_type = "Other";
        };

        logi("Device Name: {}", properties.deviceName.data());
        logi("Device Type: {}", device_type);

        auto extensions = handle->enumerateDeviceExtensionProperties();

        logv("Device can support extensions:");
        for (auto& extension : extensions)
            logv("\t{}", extension.extensionName.data());

    }

    uint32_t GPU::get_memory_index (vk::MemoryRequirements requirements, vk::MemoryPropertyFlags flags) const {

        auto properties = handle->getMemoryProperties();

        for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {

            bool supported = requirements.memoryTypeBits & (1 << i);
            bool sufficient = (properties.memoryTypes.at(i).propertyFlags & flags) == flags;

            if (supported && sufficient) return i;

        }

        return std::numeric_limits<uint32_t>::max();

    }

    const vk::SampleCountFlagBits GPU::get_samples() const {

        auto properties = handle->getProperties();

        auto color_samples_count = properties.limits.framebufferColorSampleCounts;
        auto depth_samples_count = properties.limits.framebufferDepthSampleCounts;

        auto sample_counts = color_samples_count & depth_samples_count;

        using enum vk::SampleCountFlagBits;

        auto sample_count = e1;

        if (sample_counts & e64) sample_count = e64;
        else if (sample_counts & e32) sample_count = e32;
        else if (sample_counts & e16) sample_count = e16;
        else if (sample_counts & e8) sample_count = e8;
        else if (sample_counts & e4) sample_count = e4;
        else if (sample_counts & e2) sample_count = e2;

        return sample_count;

    }

}