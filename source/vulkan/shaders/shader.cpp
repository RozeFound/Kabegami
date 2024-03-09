#include "shader.hpp"

#include "assets/shader.hpp"

namespace vku {

    Shader::Shader (const assets::FileSystem& fs, std::string path) {

        glsl::Compiler::Options options;
        auto compiler = glsl::Compiler(options);
        auto parser = assets::ShaderParser(fs, path);

        for (auto& unit : parser.get_units()) {

            std::vector<uint32_t> spriv;
            compiler.compile(unit, spriv);

            auto module = std::make_shared<vku::ShaderModule>(spriv, unit.stage);
            modules.emplace_back(module);

        }

    }

}