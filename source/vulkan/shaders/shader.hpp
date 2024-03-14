#pragma once

#include "module.hpp"
#include "assets/filesystem.hpp"

namespace vku {

    class Shader {

        std::vector<std::shared_ptr<vku::ShaderModule>> modules;

        public:

        Shader() = delete;
        Shader (const assets::FileSystem& fs, std::string path);

        constexpr const auto get_modules() const { return modules; }


    };

}