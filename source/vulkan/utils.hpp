#pragma once

#include <optional>

namespace vku {

    struct QueueFamilyIndices {
        
        std::optional<uint32_t> transfer_family;
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
        std::optional<uint32_t> compute_family;

        constexpr bool is_complete ( ) {
            if (transfer_family.has_value()
                && graphics_family.has_value()
                && present_family.has_value()
                && compute_family.has_value())
                return true;
            return false;
        }
        
	};

    constexpr auto get_image_view_info (vk::Image& image, 
        vk::Format format, vk::ImageAspectFlags flags, uint32_t mip_levels = 0) {

        return vk::ImageViewCreateInfo {
            .flags = vk::ImageViewCreateFlags(),
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = format,
            .components = vk::ComponentMapping(),
            .subresourceRange = {
                .aspectMask = flags,
                .baseMipLevel = 0,
                .levelCount = mip_levels,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

    }

    constexpr uint32_t to_u32 (std::size_t value) { return static_cast<uint32_t>(value); }

}