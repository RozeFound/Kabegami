#include "engine.hpp"

Engine::Engine (const Window& window) {

    context = std::make_shared<vki::Context>(window);
    context->set(context);

    auto [major, minor, patch] = context->get_version();
    logi("System support up to Vulkan {}.{}.{}", major, minor, patch);
    if constexpr (debug) debug_messenger = make_debug_messenger(*context->instance);

    render_pass = context->create_render_pass();
    swapchain = std::make_unique<vki::SwapChain>(*render_pass);
    image_count = swapchain->get_frames().size();
    assign_command_buffers();

    auto graphics = context->get_queue_family_indices().graphics_family.value();
    queue = std::make_unique<vk::raii::Queue>(context->device, graphics, 0);

    fps_limiter.is_enabled = true;
    fps_limiter.set_target(144);

}

Engine::~Engine() {

    context->device->waitIdle();

    auto& frames = swapchain->get_frames();

    for (auto& frame : frames) 
        frame.commands.reset();

    scene.reset();

}

void Engine::set_scene (std::shared_ptr<Scene> scene) {

    this->scene = scene;
    scene->allocate_resources(*render_pass);

};

void Engine::assign_command_buffers() {

    auto create_info = vk::CommandPoolCreateInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = context->get_queue_family_indices().graphics_family.value()
    };

    try { command_pool = std::make_unique<vk::raii::CommandPool>(*context->device, create_info); } 
    catch (vk::SystemError e) { loge("Failed to create Command Pool: {}", e.what()); return; }

    auto allocate_info = vk::CommandBufferAllocateInfo {
        .commandPool = **command_pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = image_count
    };

    auto command_buffers = vk::raii::CommandBuffers(context->device, allocate_info);
    auto& frames = swapchain->get_frames();

    for (std::size_t i = 0; i < image_count; i++)
        frames.at(i).commands = std::make_unique<vk::raii::CommandBuffer>(std::move(command_buffers.at((i))));

}

void Engine::record (uint32_t index, std::function<void()> callback) {

    const auto& frame = swapchain->get_frames().at(index);

    try { frame.commands->reset(); frame.commands->begin(vk::CommandBufferBeginInfo()); } 
    catch (vk::SystemError e) { loge("Failed to begin command record: {}", e.what()); return; }

    auto clear_values = std::array {
        vk::ClearValue { scene->clear_color },
        vk::ClearValue {},
        vk::ClearValue { .depthStencil = { 1.f, 0 } }
    };

    auto begin_info = vk::RenderPassBeginInfo {
        .renderPass = **render_pass,
        .framebuffer = **frame.buffer,
        .renderArea = {{0, 0}, swapchain->get_extent()},
        .clearValueCount = vku::to_u32(clear_values.size()),
        .pClearValues = clear_values.data()
    };

    frame.commands->beginRenderPass(begin_info, vk::SubpassContents::eInline);

     auto viewport = vk::Viewport {
        .width = static_cast<float>(swapchain->get_extent().width),
        .height = static_cast<float>(swapchain->get_extent().height),       
        .minDepth = 0.f,
        .maxDepth = 1.f
    };

    auto scissor = vk::Rect2D { .extent = swapchain->get_extent() };

    frame.commands->setViewport(0, viewport);
    frame.commands->setScissor(0, scissor);

    if (callback) callback();

    frame.commands->endRenderPass();

    try { frame.commands->end(); } catch (vk::SystemError e)
        { loge("Failed to record command buffer: {}", e.what()); }

}

void Engine::update() {

    if (swapchain->resize_if_needed()) frame_index = 0;
    fps_limiter.delay();

    auto& frame = swapchain->get_frames().at(frame_index);
    frame.index = swapchain->acquire_image(frame_index);

    record(frame_index, [&] {

        scene->draw(*frame.commands);

    });

    submit(frame_index);

    if (!swapchain->present_image(frame_index)) { frame_index = 0; return; }
    frame_index = (frame_index + 1) % image_count;

}

void Engine::submit (uint32_t index) {

    const auto& frame = swapchain->get_frames().at(index);

    vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    auto wait_semaphores = std::array { *frame.image_available };

    auto submit_info = vk::SubmitInfo {
        .waitSemaphoreCount = vku::to_u32(wait_semaphores.size()),
        .pWaitSemaphores = wait_semaphores.data(),
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &**frame.commands,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*frame.render_finished
    };

    try { queue->submit(submit_info, *frame.in_flight); } 
    catch (vk::SystemError e) { loge("Failed to submit command buffer: {}", e.what()); }

}