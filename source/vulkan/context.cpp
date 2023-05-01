#include "vulkan/context.hpp"

namespace vki {

    const vku::QueueFamilyIndices Context::get_queue_family_indices() const {

        vku::QueueFamilyIndices indices;
        auto queue_family_properties = gpu->getQueueFamilyProperties();

        for (std::size_t i = 0; i < queue_family_properties.size(); i++) {

            auto queue_flags = queue_family_properties.at(i).queueFlags;
            
            if (queue_flags & vk::QueueFlagBits::eGraphics) indices.graphics_family = i;
            if (gpu->getSurfaceSupportKHR(i, **surface)) indices.present_family = i;

            if (queue_flags & vk::QueueFlagBits::eTransfer 
                && queue_flags & ~vk::QueueFlagBits::eGraphics) 
                indices.transfer_family = i;

            if (queue_flags & vk::QueueFlagBits::eCompute
                && queue_flags & ~vk::QueueFlagBits::eGraphics) 
                indices.compute_family = i;

            if(indices.is_complete()) break;

        }

        return indices;

    }

}