#pragma once

namespace vki {

    class Surface {

        std::unique_ptr <vk::raii::SurfaceKHR> handle;

        public:

        Surface (const vk::raii::Instance& instance, GLFWwindow* window);

        constexpr const auto& get_handle() const { return *handle; }
        constexpr const auto& operator* () const { return get_handle(); }
        constexpr const auto& operator->() const { return handle; }

    };

}