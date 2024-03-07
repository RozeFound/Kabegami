#pragma once

#include "module.hpp"

namespace vku {

    class Shader {

        std::vector<std::shared_ptr<vku::ShaderModule>> modules;

        public:

        Shader() = delete;
        Shader (assets::ShaderParser parser);
        Shader (const assets::FileSystem& fs, std::string path)
            : Shader(assets::ShaderParser(path, fs)) {}

        constexpr const auto get_modules() const { return modules; }


    };

}