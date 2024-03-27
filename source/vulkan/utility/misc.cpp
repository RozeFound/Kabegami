#include "misc.hpp"

namespace vku {

    vk::ImageViewCreateInfo get_image_view_info (const vk::Image& image, 
        vk::Format format, vk::ImageAspectFlags flags, uint32_t mip_levels) {

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

}