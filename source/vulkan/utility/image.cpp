#include "image.hpp"
#include "memory.hpp"

namespace vku {

    void Image::create_handle (vk::ImageUsageFlags usage) {

        auto create_info = vk::ImageCreateInfo {
            .flags = vk::ImageCreateFlags(),
            .imageType = vk::ImageType::e2D,
            .format = format,
            .extent = {to_u32(width), to_u32(height), 1},
            .mipLevels = mip_levels,
            .arrayLayers = 1,
            .samples = vk::SampleCountFlagBits::e1,
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

    Texture::Texture (const TextureInfo& info) {

        auto header = info.get_header();
        width = header.width; height = header.height;
        format = vk::Format::eR8G8B8A8Srgb;

        auto& mipmaps = info.get_images().at(0);
        mip_levels = mipmaps.size();

        using enum vk::ImageUsageFlagBits;
        Image::create_handle(eTransferSrc | eTransferDst | eSampled);
        Image::create_view(vk::ImageAspectFlagBits::eColor);

        create_sampler();
        create_descriptors();
        update_mipmaps(mipmaps);

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

    void Texture::create_descriptors() {

        auto pool_size = vk::DescriptorPoolSize {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1
        };

        auto pool_info = vk::DescriptorPoolCreateInfo {
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = &pool_size
        };

        try { pool = std::make_unique<vk::raii::DescriptorPool>(context->device, pool_info); }
        catch (vk::SystemError e) { loge("Failed to create Descriptor Pool: {}", e.what()); }

        auto binding = vk::DescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
        };

        auto layout_info = vk::DescriptorSetLayoutCreateInfo {
            .flags = vk::DescriptorSetLayoutCreateFlags(),
            .bindingCount = 1,
            .pBindings = &binding
        };

        auto layout = vk::raii::DescriptorSetLayout(context->device, layout_info);

        auto allocate_info = vk::DescriptorSetAllocateInfo {
            .descriptorPool = **pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &*layout
        };

        try { auto sets = vk::raii::DescriptorSets(context->device, allocate_info);
              set = std::make_unique<vk::raii::DescriptorSet>(std::move(sets.back()));
        } catch (vk::SystemError e) { loge("Failed to allocate DescriptorSet's: {}", e.what()); }

        auto image_info = vk::DescriptorImageInfo {
            .sampler = **sampler,
            .imageView = **view,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
        };

        auto write_info = vk::WriteDescriptorSet {
            .dstSet = **set,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = &image_info
        };

        context->device->updateDescriptorSets(write_info, nullptr);

    }

    void Texture::update_mipmaps (const std::vector<MipMap>& mipmaps) {

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

}