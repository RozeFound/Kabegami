#pragma once

#include "assets/shader.hpp"
#include "module.hpp"

namespace vku {

    class Shader {

        public:

        std::shared_ptr<ShaderModule> vertex_module;
        std::shared_ptr<ShaderModule> fragment_module;

        Shader() = delete;
        Shader (assets::ShaderParser parser);
        Shader (std::string path, const assets::FileSystem& fs)
            : Shader(assets::ShaderParser(path, fs)) {};


    };

}