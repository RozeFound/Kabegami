#include "window.hpp"

Window::Window (std::string_view title, std::size_t width, std::size_t height) {

    auto ec = glz::read_file(settings, "window-settings.crush");

    if (!settings.width) settings.width = width;
    if (!settings.height) settings.height = height;

    logi("Creating window: {}x{}", settings.width, settings.height);

    glfwInit();

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    handle = glfwCreateWindow(settings.width, settings.height, title.data(), nullptr, nullptr);

    if (handle) logi("Successfully created {} window!", title);
    else logw("Failed to create {} window", title);

    glfwSetWindowSizeLimits(handle, width / 2, height / 2, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetWindowUserPointer(handle, this);

    set_callbacks();

}

Window::~Window() {

    glfwTerminate();

    auto ec = glz::write_file(settings, "window-settings.crush");

}

void Window::set_callbacks() {

    auto resize_callback = [](GLFWwindow* window, int width, int height) {

        auto parent = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        if (!parent->settings.fullscreen) {
            parent->settings.height = height;
            parent->settings.width = width;
        }

    };

    auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {

        auto parent = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

        for (const auto& callback : parent->callbacks)
            callback(key, action, mods);

    };

    glfwSetFramebufferSizeCallback(handle, resize_callback);
    glfwSetKeyCallback(handle, key_callback);

}

bool Window::toggle_fullscreen() {

    settings.fullscreen = !settings.fullscreen;

    if (settings.fullscreen) {

        auto monitor = glfwGetPrimaryMonitor();

        int x, y, width, height; 
        glfwGetMonitorWorkarea(monitor, &x, &y, &width, &height);

        glfwSetWindowMonitor(handle, monitor, x, y, width, height, GLFW_DONT_CARE);

    } 
    else glfwSetWindowMonitor(handle, nullptr, 0, 0, settings.width, settings.height, 0);

    return !settings.fullscreen;

}