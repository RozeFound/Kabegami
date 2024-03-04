#pragma once

#include "assets/shader.hpp"

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/MachineIndependent/iomapper.h>

namespace glsl {

    class Compiler {

        struct Options {

            glslang::EShTargetClientVersion client_version = glslang::EShTargetVulkan_1_3;

            bool hlsl = false;

            bool auto_map_locations = false;
            bool auto_map_bindings = false;

            bool relaxed_rules_vulkan = false;
            bool global_uniform_binding = false;

        };

        const TBuiltInResource* resource_limits = GetDefaultResources();
        const EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

        std::unordered_map<vk::ShaderStageFlagBits, std::vector<uint32_t>> spvs;
        
        constexpr auto get_client (glslang::EShTargetClientVersion version) {
            switch (version) {
                case glslang::EShTargetVulkan_1_0:
                case glslang::EShTargetVulkan_1_1:
                case glslang::EShTargetVulkan_1_2:
                case glslang::EShTargetVulkan_1_3: return glslang::EShClientVulkan;
                case glslang::EShTargetOpenGL_450: return glslang::EShClientOpenGL;
                default: return glslang::EShClientVulkan;
            }
        }

        constexpr auto get_target_version (glslang::EShTargetClientVersion version) {
            switch (version) {
                case glslang::EShTargetVulkan_1_0: return glslang::EShTargetSpv_1_0;
                case glslang::EShTargetVulkan_1_1: return glslang::EShTargetSpv_1_3;
                case glslang::EShTargetVulkan_1_2: return glslang::EShTargetSpv_1_5;
                case glslang::EShTargetVulkan_1_3: return glslang::EShTargetSpv_1_6;
                case glslang::EShTargetOpenGL_450: return glslang::EShTargetSpv_1_0;
                default: return glslang::EShTargetSpv_1_0;
            }
        }

        constexpr auto find_shader_language (vk::ShaderStageFlagBits stage) {
            switch (stage)
            {
                case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
                case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
                case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
                default: return EShLangVertex;
            }
        }

        public:

        bool preprocess (assets::ShaderUnit& unit, glslang::TShader& shader, Options options);
        bool parse (assets::ShaderUnit& unit, glslang::TShader& shader, Options options);
        bool compile (std::array<assets::ShaderUnit, 2>& units);

        constexpr auto get_spv (vk::ShaderStageFlagBits stage) {
            return spvs.at(stage);
        }

    };


}