#include "context.hpp"

namespace vki {

    static std::weak_ptr<Context> global_context;

    void Context::set (std::shared_ptr<Context> context) { global_context = context; }

    const std::shared_ptr<Context> Context::get() {
        if (!global_context.expired()) return global_context.lock();
        else throw std::runtime_error("Vulkan Context has been expired!");
    }

    const vku::QueueFamilyIndices Context::get_queue_family_indices() const {

        vku::QueueFamilyIndices indices;
        auto queue_family_properties = gpu->getQueueFamilyProperties();

        for (std::size_t i = 0; i < queue_family_properties.size(); i++) {

            auto queue_flags = queue_family_properties.at(i).queueFlags;
            
            if (queue_flags & vk::QueueFlagBits::eGraphics) indices.graphics_family = i;
            if (gpu->getSurfaceSupportKHR(i, **surface)) indices.present_family = i;

            if (queue_flags & vk::QueueFlagBits::eTransfer && i != indices.graphics_family)
                indices.transfer_family = i;

            if (queue_flags & vk::QueueFlagBits::eCompute && i != indices.graphics_family)
                indices.compute_family = i;

            if(indices.is_complete()) break;

        }

        return indices;

    }

    const vk::Extent2D Context::get_extent() const {

        auto capabilities = gpu->getSurfaceCapabilitiesKHR(**surface);

        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
            
        int width, height;
        glfwGetFramebufferSize(*window, &width, &height);

        auto min_width = std::max(capabilities.minImageExtent.width, vku::to_u32(width));
        auto min_height = std::max(capabilities.minImageExtent.height, vku::to_u32(height));

        return vk::Extent2D { 
            .width = std::min(min_width, capabilities.maxImageExtent.width),
            .height = std::min(min_height, capabilities.maxImageExtent.height)
        };

    } 

    const vk::SurfaceFormatKHR Context::get_format() const {

        auto formats = gpu->getSurfaceFormatsKHR(**surface);

        for (const auto& format : formats)
            if (format.format == vk::Format::eB8G8R8A8Unorm 
                && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
                return format;

        return formats.at(0);

    }

    const vk::Format Context::get_depth_format() const {

        auto candidates = { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint };

        for (const auto& format : candidates) {

            auto properties = gpu->getFormatProperties(format);

            if (properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
                return format;
        }

        throw std::runtime_error("Failed to retrieve Depth Format");

    }

    const vku::Version Context::get_version() const {

        auto version = handle.enumerateInstanceVersion();

        return vku::Version {
            .major = VK_API_VERSION_MAJOR(version),
            .minor = VK_API_VERSION_MINOR(version),
            .patch = VK_API_VERSION_PATCH(version)
        };

    }

    std::unique_ptr<vk::raii::RenderPass> Context::create_render_pass() {

        auto sample_count = gpu.get_samples();

        auto color_attachment = vk::AttachmentDescription {
            .flags = vk::AttachmentDescriptionFlags(),
            .format = get_format().format,
            .samples = sample_count,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = vk::ImageLayout::eUndefined,
            .finalLayout = vk::ImageLayout::eColorAttachmentOptimal
        };

        auto color_attachment_reference = vk::AttachmentReference {
            .attachment = 0,
            .layout = vk::ImageLayout::eColorAttachmentOptimal
        };

        auto resolve_attachment = vk::AttachmentDescription {
            .flags = vk::AttachmentDescriptionFlags(),
            .format = get_format().format,
            .samples = vk::SampleCountFlagBits::e1,
            .loadOp = vk::AttachmentLoadOp::eDontCare,
            .storeOp = vk::AttachmentStoreOp::eStore,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = vk::ImageLayout::eUndefined,
            .finalLayout = vk::ImageLayout::ePresentSrcKHR
        };

        auto resolve_attachment_reference = vk::AttachmentReference {
            .attachment = 1,
            .layout = vk::ImageLayout::eColorAttachmentOptimal
        };

        auto depth_attachment = vk::AttachmentDescription {
            .flags = vk::AttachmentDescriptionFlags(),
            .format = get_depth_format(),
            .samples = sample_count,
            .loadOp = vk::AttachmentLoadOp::eClear,
            .storeOp = vk::AttachmentStoreOp::eDontCare,
            .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
            .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
            .initialLayout = vk::ImageLayout::eUndefined,
            .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal
        };

        auto depth_attachment_reference = vk::AttachmentReference {
            .attachment = 2,
            .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal
        };

        auto subpass = vk::SubpassDescription {
            .flags = vk::SubpassDescriptionFlags(),
            .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment_reference,
            .pResolveAttachments = &resolve_attachment_reference,
            .pDepthStencilAttachment = &depth_attachment_reference
        };

        auto subpass_dependencies = std::array {
            vk::SubpassDependency {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
                .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests,
                .srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
                .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead
            },
            vk::SubpassDependency {
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
                .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead
            }
        };

        auto attachments = std::array { color_attachment, resolve_attachment, depth_attachment };

        auto create_info = vk::RenderPassCreateInfo {
            .flags = vk::RenderPassCreateFlags(),
            .attachmentCount = vku::to_u32(attachments.size()),
            .pAttachments = attachments.data(),
            .subpassCount = 1,
            .pSubpasses = &subpass,
            .dependencyCount = vku::to_u32(subpass_dependencies.size()),
            .pDependencies = subpass_dependencies.data()
        };

        try { return std::make_unique<vk::raii::RenderPass>(device, create_info); }
        catch (const vk::SystemError& e) { loge("Failed to create render pass: {}", e.what()); }
        return nullptr;

    }

}