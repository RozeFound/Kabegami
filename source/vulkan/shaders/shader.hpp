#pragma once

#include "module.hpp"
#include "assets/filesystem.hpp"

namespace vku {

    class Shader {

        std::vector<std::shared_ptr<vku::ShaderModule>> modules;
        std::shared_ptr<vk::raii::DescriptorSetLayout> descriptor_set_layout;

        public:

        Shader() = delete;
        Shader (const assets::FileSystem& fs, std::string path);

        constexpr const auto& get_modules() const { return modules; }
        constexpr const auto& get_layout() const { return descriptor_set_layout; }


    };

}