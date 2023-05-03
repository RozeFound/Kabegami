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

}