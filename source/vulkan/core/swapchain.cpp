#include "swapchain.hpp"

#include "vulkan/utility/misc.hpp"

namespace vki {

    void SwapChain::create_handle() {

        auto capabilities = context->gpu->getSurfaceCapabilitiesKHR(context->surface);
        auto modes = context->gpu->getSurfacePresentModesKHR(context->surface);
        extent = context->get_extent();

        auto present_mode = vk::PresentModeKHR::eFifo;

        if constexpr (debug) for (auto& mode : modes)
            if (mode == vk::PresentModeKHR::eMailbox)
                { present_mode = mode; break; }

        auto image_count = capabilities.minImageCount + 1;

        if (image_count > capabilities.maxImageCount)
            image_count = capabilities.maxImageCount;

        auto create_info = vk::SwapchainCreateInfoKHR {
            .flags = vk::SwapchainCreateFlagsKHR(),
            .surface = context->surface, 
            .minImageCount = image_count,
            .imageFormat = context->get_format().format,
            .imageColorSpace = context->get_format().colorSpace,
            .imageExtent = extent,
            .imageArrayLayers = 1,
            .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
            // We'll set sharing mode and queue indices below //
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eInherit,
            .presentMode = present_mode,
            .clipped = true
        };

        if (handle) create_info.oldSwapchain = **handle;

        auto indices = context->get_queue_family_indices();
        uint32_t queue_family_indices[] = { 
            indices.graphics_family.value(), 
            indices.present_family.value()
        };

        if (indices.graphics_family != indices.present_family) {
                create_info.imageSharingMode = vk::SharingMode::eConcurrent;
                create_info.queueFamilyIndexCount = 2;
                create_info.pQueueFamilyIndices = queue_family_indices;
        } else create_info.imageSharingMode = vk::SharingMode::eExclusive;

        try { handle = std::make_unique<vk::raii::SwapchainKHR>(context->device, create_info); }
        catch (vk::SystemError) { loge("Failed to create SwapChain"); return; }
        logi("Successfully created SwapChain");

    }

    void SwapChain::make_frames() {

        auto images = handle->getImages();

        for (std::size_t i = 0; i < images.size(); i++) {

            auto format = context->get_format().format;

            auto view_info = vku::get_image_view_info(images.at(i), format, vk::ImageAspectFlagBits::eColor);
            auto semaphore_info = vk::SemaphoreCreateInfo { .flags = vk::SemaphoreCreateFlags() };
            auto fence_info = vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled };

            auto frame = Frame {
                .image = images.at(i),
                .view = vk::raii::ImageView(context->device, view_info),
                .image_available = vk::raii::Semaphore(context->device, semaphore_info),
                .render_finished = vk::raii::Semaphore(context->device, semaphore_info),
                .in_flight = vk::raii::Fence(context->device, fence_info)
            };

            auto attachments = std::array { *frame.view };

            auto buffer_info = vk::FramebufferCreateInfo {
                .flags = vk::FramebufferCreateFlags(),
                .renderPass = *render_pass,
                .attachmentCount = vku::to_u32(attachments.size()),
                .pAttachments = attachments.data(),
                .width = extent.width,
                .height = extent.height,
                .layers = 1
            };

            try { frame.buffer = std::make_unique<vk::raii::Framebuffer>(context->device, buffer_info); } 
            catch (vk::SystemError e) { loge("Failed to create Framebuffer: {}", e.what()); return; }

            if (frames.size() > i) {
                frame.commands = std::move(frames.at(i).commands);
                frames.at(i) = std::move(frame);
            } else frames.emplace_back(std::move(frame));

		};

        logi("Successfully created Framebuffers");
        logi("Created ImageView's for SwapChain");
        logi("Created syncronization structures");

    }

    uint32_t SwapChain::acquire_image (uint32_t index) {

        constexpr auto timeout = std::numeric_limits<uint64_t>::max();
        const auto& frame = frames.at(index);

        auto wait = context->device->waitForFences(*frame.in_flight, true, timeout);
        if (wait != vk::Result::eSuccess) logw("Something goes wrong when waiting on fences");
        context->device->resetFences(*frame.in_flight);

        auto [result, image_index] = handle->acquireNextImage(timeout, *frame.image_available);

        if (result == vk::Result::eErrorOutOfDateKHR) resize_if_needed();

        return image_index;

    }

    bool SwapChain::present_image (uint32_t index) {

        const auto& frame = frames.at(index);

        auto present_info = vk::PresentInfoKHR {
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*frame.render_finished,
            .swapchainCount = 1,
            .pSwapchains = &**handle,
            .pImageIndices = &frame.index
        };

        try { auto result = queue->presentKHR(present_info); } 
        catch (vk::OutOfDateKHRError e) { resize_if_needed(); return false; }
        return true;

    }

    bool SwapChain::resize_if_needed() {
        
        auto new_extent = context->get_extent();

        if (new_extent == extent) return false;
        context->device->waitIdle();
        create_handle();
        make_frames();

        return true;

    }

}