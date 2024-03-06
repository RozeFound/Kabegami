#include "shader.hpp"

namespace vku {

    Shader::Shader (assets::ShaderParser parser) {

        auto compiler = glsl::Compiler();
        compiler.compile(parser.get_units());

        vertex_module = std::make_shared<ShaderModule>(compiler.get_spv(vk::ShaderStageFlagBits::eVertex));
        fragment_module = std::make_shared<ShaderModule>(compiler.get_spv(vk::ShaderStageFlagBits::eFragment));

    }

}