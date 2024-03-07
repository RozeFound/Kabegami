#include "shader.hpp"

#include "assets/shader.hpp"

namespace vku {

    Shader::Shader (assets::ShaderParser parser) {

        glsl::Compiler::Options options;
        options.auto_map_locations = true;
        options.auto_map_bindings = true;

        auto compiler = glsl::Compiler(options);

        for (auto& unit : parser.get_units()) {

            std::vector<uint32_t> spriv;
            compiler.compile(unit, spriv);

            auto module = std::make_shared<vku::ShaderModule>(spriv, unit.stage);
            modules.emplace_back(module);

        }

    }

}