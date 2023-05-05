#pragma once

namespace vki {

    class GPU {

        std::unique_ptr <vk::raii::PhysicalDevice> handle;

        public:

        void log_properties();
        uint32_t get_memory_index (vk::MemoryRequirements, vk::MemoryPropertyFlags) const;

        GPU (const vk::raii::Instance&  instance);

        constexpr const auto& get_handle() const { return *handle; }
        constexpr const auto& operator* () const { return get_handle(); }
        constexpr const auto& operator->() const { return handle; }

    };

}