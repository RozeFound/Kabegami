#pragma once

#include "vulkan/core/context.hpp"
#include "assets/texture.hpp"

namespace vku {

    class Image {

        protected:

        std::size_t width, height;
        uint32_t mip_levels = 1;
        vk::Format format;

        std::unique_ptr <vk::raii::Image> handle;
        std::unique_ptr <vk::raii::ImageView> view;
        std::unique_ptr <vk::raii::DeviceMemory> memory;

        std::shared_ptr <vki::Context> context = vki::Context::get();

        void create_handle (vk::ImageUsageFlags usage);
        void create_view (vk::ImageAspectFlags flags);

        public:

        Image() = default;
        Image (std::size_t width, std::size_t height, vk::ImageUsageFlags usage)
            : width(width), height(height), mip_levels(1) { create_handle(usage); }

    };

    class Texture : public Image {

        std::unique_ptr<vk::raii::Sampler> sampler;

        std::unique_ptr<vk::raii::DescriptorPool> pool;
        std::unique_ptr<vk::raii::DescriptorSet> set;

        void create_sampler();
        void create_descriptors();
        void update_mipmaps (const std::vector<MipMap>& mipmaps);

        public:

        Texture (const TextureInfo& info);
        ~Texture() { set.reset(); }

        constexpr const auto& get_descriptor_set() const { return set; }
        
    };

}