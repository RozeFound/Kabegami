#pragma once

#include <cstddef>

#include <glaze/glaze.hpp>

class Window {

    struct Settings {
        bool fullscreen;
        std::size_t width, height;
    } settings;

    GLFWwindow* handle;

    std::vector<std::function<void(int&, int&, int&)>> callbacks;
    void set_callbacks();

    public:

    Window (std::string_view title, std::size_t width, std::size_t height);
    Window (const Window&) = delete;
    ~Window();

    constexpr const auto& operator* () const { return handle; }

    constexpr bool is_open() const noexcept {
        auto should_close = glfwWindowShouldClose(handle);
        if (!should_close) glfwPollEvents();
        return !should_close;
    }

    constexpr void close() { glfwSetWindowShouldClose(handle, true); }

    void add_key_callback (std::function<void(int&, int&, int&)> callback) { callbacks.push_back(callback); }

    bool toggle_fullscreen();

};