#include "context.hpp"

namespace vki {

    static std::weak_ptr<Context> global_context;

    void Context::set (std::shared_ptr<Context> context) { global_context = context; }

    const std::shared_ptr<Context> Context::get() {
        if (!global_context.expired()) return global_context.lock();
        else throw std::runtime_error("Vulkan Context has been expired!");
    }

    const vku::QueueFamilyIndices Context::get_queue_family_indices() const {

        vku::QueueFamilyIndices indices;
        auto queue_family_properties = gpu->getQueueFamilyProperties();

        for (std::size_t i = 0; i < queue_family_properties.size(); i++) {

            auto queue_flags = queue_family_properties.at(i).queueFlags;
            
            if (queue_flags & vk::QueueFlagBits::eGraphics) indices.graphics_family = i;
            if (gpu->getSurfaceSupportKHR(i, **surface)) indices.present_family = i;

            if (queue_flags & vk::QueueFlagBits::eTransfer && i != indices.graphics_family)
                indices.transfer_family = i;

            if (queue_flags & vk::QueueFlagBits::eCompute && i != indices.graphics_family)
                indices.compute_family = i;

            if(indices.is_complete()) break;

        }

        return indices;

    }

    const vk::Extent2D Context::get_extent() const {

        auto capabilities = gpu->getSurfaceCapabilitiesKHR(**surface);

        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
            
        int width, height;
        glfwGetFramebufferSize(*window, &width, &height);

        auto min_width = std::max(capabilities.minImageExtent.width, vku::to_u32(width));
        auto min_height = std::max(capabilities.minImageExtent.height, vku::to_u32(height));

        return vk::Extent2D { 
            .width = std::min(min_width, capabilities.maxImageExtent.width),
            .height = std::min(min_height, capabilities.maxImageExtent.height)
        };

    } 

    const vk::SurfaceFormatKHR Context::get_format() const {

        auto formats = gpu->getSurfaceFormatsKHR(**surface);

        for (const auto& format : formats)
            if (format.format == vk::Format::eB8G8R8A8Unorm 
                && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return format;

        return formats.at(0);

    }

}