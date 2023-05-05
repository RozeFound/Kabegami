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

        logi("Device Name: {}", properties.deviceName);
        logi("Device Type: {}", device_type);

        auto extensions = handle->enumerateDeviceExtensionProperties();

        logv("Device can support extensions:");
        for (auto& extension : extensions)
            logv("\t{}", extension.extensionName);

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

}