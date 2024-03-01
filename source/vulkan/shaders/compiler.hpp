#pragma once

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

namespace glsl {

    class Compiler {
        
        vk::ShaderStageFlagBits stage;
        std::vector<uint32_t> spirv;
        std::string log;

        static constexpr EShLanguage find_shader_language(vk::ShaderStageFlagBits stage);

        public:

        Compiler(vk::ShaderStageFlagBits stage) : stage(stage) {}

        bool compile(std::string_view source);

        constexpr const auto& get_spirv() const { return spirv; }
        constexpr const auto& get_log() const { return log; }

    };


}