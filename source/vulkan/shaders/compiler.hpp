#pragma once

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

#include "includer.hpp"

namespace glsl {

    class Compiler {
        
        vk::ShaderStageFlagBits stage;
        std::vector<uint32_t> spirv;
        std::string log;

        Includer includer;

        public:

        Compiler(vk::ShaderStageFlagBits stage, const assets::FileSystem& fs) : stage(stage), includer(fs) {}

        bool compile(std::vector<uint8_t> source);

        constexpr const auto& get_spirv() const { return spirv; }
        constexpr const auto& get_log() const { return log; }

    };


}