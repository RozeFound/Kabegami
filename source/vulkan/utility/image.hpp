#pragma once

#include "vulkan/core/context.hpp"
#include "assets/texture.hpp"
#include "transient.hpp"

namespace vku {

    class Image {

        protected:

        std::size_t width, height;
        uint32_t mip_levels = 1;
        vk::Format format;

        std::unique_ptr<vk::raii::Image> handle;
        std::unique_ptr<vk::raii::ImageView> view;
        std::unique_ptr<vk::raii::DeviceMemory> memory;

        vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        void create_handle (vk::ImageUsageFlags usage);
        void create_view (vk::ImageAspectFlags flags);

        public:

        Image() = default;
        Image (std::size_t width, std::size_t height, vk::Format format, vk::ImageUsageFlags usage)
            : width(width), height(height), mip_levels(1), format(format) { 

            samples = context->gpu.get_samples();
            
            create_handle(usage); 

            auto image_aspect = vk::ImageAspectFlagBits::eColor;
            if (usage == vk::ImageUsageFlagBits::eDepthStencilAttachment)
                image_aspect = vk::ImageAspectFlagBits::eDepth;

            create_view(image_aspect);
        }

        constexpr const auto& get_view() const { return *view; }

    };

    class Texture : public Image {

        std::unique_ptr<vk::raii::Sampler> sampler;

        void create_sampler();
        void update_mipmaps (const std::vector<assets::MipMap>& mipmaps);
        void generate_mipmaps (const TransientBuffer& commands);

        public:

        Texture (const assets::TextureParser& parser);
        Texture (std::filesystem::path path);
        Texture (Texture&&) = default;

        void write_descriptors (vk::raii::DescriptorSet& set, uint32_t binding);
        void set_data (std::span<std::byte> pixels);
        
    };

}