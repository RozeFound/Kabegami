#pragma once

namespace vki {

    class Instance {

        std::unique_ptr <vk::raii::Instance> handle;

        std::vector <const char*> get_extensions();
        std::vector <const char*> get_layers();

        void log_properties();

        public:

        Instance (const vk::raii::Context& context);

        constexpr const auto& get_handle() const { return *handle; }
        constexpr const auto& operator* () const { return get_handle(); }

    };

}