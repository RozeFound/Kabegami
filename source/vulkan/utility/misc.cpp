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

    namespace hash {

        XXH64_hash_t XXH3(std::span<std::byte> data) {
            return XXH3_64bits(data.data(), data.size());
        }

        XXH64_hash_t XXH3(std::filesystem::path path) {

			std::ifstream file;
			file.open(path, std::ios::binary);

			auto state = XXH3_createState();

			constexpr size_t buffer_size = 1024;
			auto buffer = std::array<std::byte, buffer_size>();

			XXH3_64bits_reset(state);

			do {
				file.read((char*)buffer.data(), buffer_size);
				XXH3_64bits_update(state, (xxh_u8*)buffer.data(), file.gcount());
			} while (file);

			file.close();

			auto hash = XXH3_64bits_digest(state);
			XXH3_freeState(state);

			return hash;
		}
    
    }

}