#pragma once

#include <memory>

#include "core/context.hpp"
#include "core/swapchain.hpp"
#include "core/pipeline.hpp"

#include "utility/memory.hpp"

#include "window.hpp"

class Engine {

    std::shared_ptr<vki::Context> context;
    std::unique_ptr<vki::SwapChain> swapchain;

    std::unique_ptr<vk::raii::Pipeline> pipeline;
    std::unique_ptr<vk::raii::PipelineLayout> pipeline_layout;
    std::unique_ptr<vku::PipeLineCache> pipeline_cache;
    
    std::unique_ptr<vk::raii::RenderPass> render_pass;
    std::unique_ptr<vk::raii::CommandPool> command_pool;
    std::unique_ptr<vk::raii::Queue> queue;

    std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> debug_messenger;

    uint32_t frame_index = 0;
    uint32_t image_count = 0;

    void assign_command_buffers();

    void record (uint32_t index, std::function<void()> callback);
    void submit (uint32_t index);

    public:

    std::unique_ptr<vku::DeviceBuffer> vertex_buffer;

    void update();

    Engine (const Window& window);
    ~Engine();

};