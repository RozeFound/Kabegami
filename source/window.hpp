#pragma once

#include <string>
#include <cstddef>

#include <GLFW/glfw3.h>
#include <glaze/glaze.hpp>
#include <glaze/core/macros.hpp>

class Window {

    struct Settings {
        bool fullscreen;
        std::size_t width, height;
        GLZ_LOCAL_META(Settings, width, height, fullscreen);
    } settings;

    GLFWwindow* handle;

    std::vector<std::function<void(int&, int&, int&)>> callbacks;
    void set_callbacks();

    public:

    Window (std::string_view title, std::size_t width, std::size_t height);
    Window (const Window&) = delete;
    ~Window();

    constexpr const GLFWwindow* get_handle() const noexcept { return handle; }
    constexpr GLFWwindow* get_handle() noexcept { return handle; }

    constexpr bool is_open() const noexcept {
        auto should_close = glfwWindowShouldClose(handle);
        if (!should_close) glfwPollEvents();
        return !should_close;
    }

    constexpr void close() { glfwSetWindowShouldClose(handle, true); }

    void add_key_callback (std::function<void(int&, int&, int&)> callback) { callbacks.push_back(callback); }

    const auto get_vulkan_extensions() const {

        if (!glfwVulkanSupported()) loge("Vulkan not supported");

        uint32_t glfw_extension_count = 0;
        auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        auto extensions = std::vector(glfw_extensions, glfw_extensions + glfw_extension_count);
        if (!glfw_extensions) logw("Failed to fetch required GLFW Extensions!");

        return extensions;

    }

    bool toggle_fullscreen();

};