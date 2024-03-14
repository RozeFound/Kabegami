#include "shader.hpp"

#include "assets/shader.hpp"

namespace vku {

    Shader::Shader (const assets::FileSystem& fs, std::string path) {

        glsl::Compiler::Options options;

        options.auto_map_locations = true;
        options.auto_map_bindings = true;
        options.relaxed_rules_vulkan = true;
        options.relaxed_errors_glsl = true;
        options.suppress_warnings_glsl = true;

        auto compiler = glsl::Compiler(options);
        auto parser = assets::ShaderParser(fs, path);

        auto spvs = std::vector<glsl::SPV>(); 

        if (!compiler.compile(parser.get_units(), spvs))
            loge("Failed to compile shader: {}", path);

        for (auto& spv : spvs)
            modules.emplace_back(std::make_shared<vku::ShaderModule>(spv));

    }

}