#include "vulkan/surface.hpp"

namespace vki {

    Surface::Surface (const vk::raii::Instance& instance, GLFWwindow* window) {

        auto surface = VkSurfaceKHR();
        auto result = glfwCreateWindowSurface(*instance, window, nullptr, &surface);
        if (result != VK_SUCCESS) loge("Cannot abstract GLFW surface for Vulkan");

        handle = std::make_unique<vk::raii::SurfaceKHR>(instance, surface);

    }

}