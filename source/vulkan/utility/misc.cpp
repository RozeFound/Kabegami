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

    namespace fs {

        std::vector <std::byte> read (std::filesystem::path path) {

            auto file = std::ifstream(path, std::ios::ate | std::ios::binary);

            std::size_t size = file.tellg(); file.seekg(0);
            auto buffer = std::vector<std::byte>(size);

            file.read(reinterpret_cast<char*>(buffer.data()), size);

            return buffer;

        }

        void write (std::filesystem::path path, std::span <std::byte> data) {
            auto file = std::ofstream(path, std::ios::binary);
            file.write(reinterpret_cast<char*>(data.data()), data.size());
        }

    }

}