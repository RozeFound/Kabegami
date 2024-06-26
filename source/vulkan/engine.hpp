#pragma once

#include <memory>

#include "core/context.hpp"
#include "core/swapchain.hpp"

#include "utility/fps_limiter.hpp"

#include "scene.hpp"
#include "window.hpp"

class Engine {

    std::shared_ptr<Scene> scene;
    std::shared_ptr<vki::Context> context;
    std::unique_ptr<vki::SwapChain> swapchain;
    
    std::unique_ptr<vk::raii::RenderPass> render_pass;
    std::unique_ptr<vk::raii::CommandPool> command_pool;
    std::unique_ptr<vk::raii::Queue> queue;

    std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> debug_messenger;
    vku::FPSLimiter fps_limiter;

    uint32_t frame_index = 0;
    uint32_t image_count = 0;

    void assign_command_buffers();

    void record (uint32_t index, std::function<void()> callback);
    void submit (uint32_t index);

    public:

    void update();

    void set_scene (std::shared_ptr<Scene> scene);

    Engine (const Window& window);
    ~Engine();

};