#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "vulkan/utility/memory.hpp"


namespace vku {

    void Image::create_handle (vk::ImageUsageFlags usage) {

        auto sample_count = context->gpu.get_samples();

        auto create_info = vk::ImageCreateInfo {
            .flags = vk::ImageCreateFlags(),
            .imageType = vk::ImageType::e2D,
            .format = format,
            .extent = {to_u32(width), to_u32(height), 1},
            .mipLevels = mip_levels,
            .arrayLayers = 1,
            .samples = samples,
            .tiling = vk::ImageTiling::eOptimal,
            .usage = usage,
            .sharingMode = vk::SharingMode::eExclusive,
            .initialLayout = vk::ImageLayout::eUndefined
        };

        try { handle = std::make_unique<vk::raii::Image>(context->device, create_info); }
        catch (vk::SystemError e) { loge("Failed to create Image: {}", e.what()); }

        auto requirements = handle->getMemoryRequirements();

        auto allocation_info = vk::MemoryAllocateInfo {
            .allocationSize = requirements.size,
            .memoryTypeIndex = context->gpu.get_memory_index(requirements, vk::MemoryPropertyFlagBits::eDeviceLocal)
        };

        try { memory = std::make_unique<vk::raii::DeviceMemory>(context->device, allocation_info); }
        catch (const vk::SystemError& e) { loge("Memory allocation failed: {}", e.what()); }

        handle->bindMemory(**memory, 0);

    }

    void Image::create_view (vk::ImageAspectFlags flags) {

        auto create_info = get_image_view_info(**handle, format, flags);
        view = std::make_unique<vk::raii::ImageView>(context->device, create_info);

    }

    Texture::Texture (const assets::TextureParser& parser) {

        auto header = parser.get_header();
        width = header.width; height = header.height;
        format = vk::Format::eR8G8B8A8Srgb;

        auto& mipmaps = parser.get_images().at(0);
        mip_levels = mipmaps.size();

        using enum vk::ImageUsageFlagBits;
        Image::create_handle(eTransferSrc | eTransferDst | eSampled);
        Image::create_view(vk::ImageAspectFlagBits::eColor);

        create_sampler();
        update_mipmaps(mipmaps);

    }

    Texture::Texture (std::filesystem::path path) {

        int32_t width, height, channels;
        auto stbi_image = stbi_load(path.c_str(), &width, &height, &channels, 4);
        auto pixels = reinterpret_cast<std::byte*>(stbi_image);

        this->width = width; this->height = height;
        format = vk::Format::eR8G8B8A8Srgb;

        mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

        using enum vk::ImageUsageFlagBits;
        Image::create_handle(eTransferSrc | eTransferDst | eSampled);
        Image::create_view(vk::ImageAspectFlagBits::eColor);

        create_sampler();

        auto size = width * height * 4;
        set_data({pixels, pixels + size});
        stbi_image_free(pixels);

    }

    void Texture::create_sampler() {

        auto properties = context->gpu->getProperties();

        auto create_info = vk::SamplerCreateInfo {
            .flags = vk::SamplerCreateFlags(),
            .magFilter = vk::Filter::eLinear,
            .minFilter = vk::Filter::eLinear,
            .mipmapMode = vk::SamplerMipmapMode::eLinear,
            .addressModeU = vk::SamplerAddressMode::eRepeat,
            .addressModeV = vk::SamplerAddressMode::eRepeat,
            .addressModeW = vk::SamplerAddressMode::eRepeat,
            .mipLodBias = 0.f,
            .anisotropyEnable = true,
            .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
            .compareEnable = false,
            .compareOp = vk::CompareOp::eAlways,
            .minLod = -0.f,
            .maxLod = static_cast<float>(mip_levels),
            .borderColor = vk::BorderColor::eFloatOpaqueBlack,
            .unnormalizedCoordinates = false
        };

        sampler = std::make_unique<vk::raii::Sampler>(context->device, create_info);

    }

    void Texture::write_descriptors (vk::raii::DescriptorSet& set, uint32_t binding) {

        auto image_info = vk::DescriptorImageInfo {
            .sampler = **sampler,
            .imageView = **view,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        auto write_info = vk::WriteDescriptorSet {
            .dstSet = *set,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &image_info
        };

        context->device->updateDescriptorSets(write_info, nullptr);

    }

    void Texture::update_mipmaps (const std::vector<assets::MipMap>& mipmaps) {

        auto buffers = std::vector<BasicBuffer>();
        auto commands = TransientBuffer(true);

        commands.barrier(*handle, vk::ImageAspectFlagBits::eColor, 
            { vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer },
            { vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite },
            { vk::ImageLayout::eUndefined,  vk::ImageLayout::eTransferDstOptimal }, mip_levels
        );

        for (uint32_t i = 0; i < mip_levels; i++) {

            // #NOTE Not sure if additional barriers are needed here.

            const auto& mip = mipmaps.at(i); 
            auto size = mip.width * mip.height * 4;

            buffers.emplace_back(size, vk::BufferUsageFlagBits::eTransferSrc);
            buffers.back().upload(mip.pixels.data(), mip.size);

            auto subres_layers = vk::ImageSubresourceLayers {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .mipLevel = i,
                .baseArrayLayer = 0,
                .layerCount = 1
            };

            auto region = vk::BufferImageCopy {
                .bufferOffset = 0,
                .bufferRowLength = 0,
                .bufferImageHeight = 0,
                .imageSubresource = subres_layers,
                .imageOffset = { 0, 0, 0 },
                .imageExtent = { mip.width, mip.height, 1 }
            };

            commands->copyBufferToImage(**buffers.back(), **handle, vk::ImageLayout::eTransferDstOptimal, region);

        }

        commands.barrier(*handle, vk::ImageAspectFlagBits::eColor, 
            { vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader },
            { vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead },
            { vk::ImageLayout::eTransferDstOptimal,  vk::ImageLayout::eShaderReadOnlyOptimal }, mip_levels
        );

    }

    void Texture::set_data (std::span<std::byte> pixels) {

        auto staging = BasicBuffer(pixels.size(), vk::BufferUsageFlagBits::eTransferSrc);
        staging.upload(pixels.data(), pixels.size());
        
        auto commands = TransientBuffer(true);

        commands.barrier(*handle, vk::ImageAspectFlagBits::eColor,
            { vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer },
            { vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite },
            { vk::ImageLayout::eUndefined,  vk::ImageLayout::eTransferDstOptimal }, mip_levels
        );        

        auto subres_layers = vk::ImageSubresourceLayers {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        auto region = vk::BufferImageCopy {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = subres_layers,
            .imageOffset = { 0, 0, 0 },
            .imageExtent = { to_u32(width), to_u32(height), 1 }
        };

        commands->copyBufferToImage(**staging, **handle, vk::ImageLayout::eTransferDstOptimal, region);

        generate_mipmaps(commands);

    }

    void Texture::generate_mipmaps (const TransientBuffer& commands) {

        auto barrier = vk::ImageMemoryBarrier {
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = *handle,
            .subresourceRange = {
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        int32_t mip_width = std::make_signed_t<uint32_t>(to_u32(width));
        int32_t mip_height = std::make_signed_t<uint32_t>(to_u32(height));

        for (uint32_t i = 1; i < mip_levels; i++) {

            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;

            commands.barrier(barrier, { vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer });

            auto blit = vk::ImageBlit {
                .srcSubresource = {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = i - 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .srcOffsets = { { 0, 0, 0, mip_width, mip_height, 1 } },
                .dstSubresource = {
                    .aspectMask = vk::ImageAspectFlagBits::eColor,
                    .mipLevel = i,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .dstOffsets ={ { 0, 0, 0, mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 } }
            };

            commands->blitImage(
                *handle, vk::ImageLayout::eTransferSrcOptimal,
                *handle, vk::ImageLayout::eTransferDstOptimal,
                blit, vk::Filter::eLinear
            );

            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            commands.barrier(barrier, { vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader });

            if (mip_width > 1) mip_width /= 2;
            if (mip_height > 1) mip_height /= 2;

        }

        barrier.subresourceRange.baseMipLevel = mip_levels - 1;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        commands.barrier(barrier, { vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader });

    }



}