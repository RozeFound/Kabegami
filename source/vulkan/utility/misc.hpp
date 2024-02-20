#pragma once

#include <optional>
#include <filesystem>
#include <fstream>

namespace vku {

    struct Version { uint32_t major, minor, patch; };

    struct QueueFamilyIndices {
        
        std::optional<uint32_t> transfer_family;
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
        std::optional<uint32_t> compute_family;

        constexpr bool is_complete() {
            if (transfer_family.has_value()
                && graphics_family.has_value()
                && present_family.has_value()
                && compute_family.has_value())
                return true;
            return false;
        }
        
	};

    vk::ImageViewCreateInfo get_image_view_info (const vk::Image& image, 
        vk::Format format, vk::ImageAspectFlags flags, uint32_t mip_levels = 1);

    constexpr uint32_t to_u32 (std::size_t value) { return static_cast<uint32_t>(value); }

    namespace fs {

        template <class T> concept is_array_like = requires (T cls) { cls.size(); cls.data(); };

        template <typename T = std::byte> std::vector<T> read (std::filesystem::path path) {

            auto file = std::ifstream(path, std::ios::ate | std::ios::binary);

            std::size_t size = file.tellg(); file.seekg(0);
            auto buffer = std::vector<T>(size);

            file.read(reinterpret_cast<char*>(buffer.data()), size);

            return buffer;
        }

        template <class T> requires is_array_like<T>
        void write (std::filesystem::path path, T data) {
            auto file = std::ofstream(path, std::ios::trunc | std::ios::binary);
            file.write(reinterpret_cast<char*>(data.data()), data.size());
        }
    }

}