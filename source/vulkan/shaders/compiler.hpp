#pragma once

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

namespace glsl {

    class ShaderUnit {

        std::string preamble;
        std::vector<std::string> processes;

        public:

        std::string source;
        const vk::ShaderStageFlagBits stage;

        ShaderUnit(const std::string& source, vk::ShaderStageFlagBits stage) : source(source), stage(stage) {}

        void define (const std::string& name) {
            preamble += "#define " + name + "\n";
            processes.emplace_back("D" + name);
        }

        void define (const std::string& name, const std::string& value) {
            preamble += "#define " + name + " " + value + "\n";
            processes.emplace_back("D" + name + "=" + value);
        }
        void undefine (const std::string& name) {
            preamble += "#undef " + name + "\n";
            processes.emplace_back("U" + name);
        }

        constexpr const auto get_preamble() const { return preamble; }
        constexpr const auto get_processes() const { return processes; }

    };

    class Compiler {

        const TBuiltInResource* resource_limits = GetDefaultResources();
        const EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);
        
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

        public:

        struct Options {

            glslang::EShTargetClientVersion client_version = glslang::EShTargetVulkan_1_3;

            bool hlsl = false;

            bool auto_map_locations = false;
            bool auto_map_bindings = false;

            bool relaxed_rules_vulkan = false;
            bool global_uniform_binding = false;

            bool optimize = false;
            bool optimize_size = false;
            
        } const options;

        Compiler() = default;
        Compiler (Options options) : options(options) {};

        bool parse (ShaderUnit& unit, glslang::TShader& shader);
        bool compile (ShaderUnit& unit, std::vector<uint32_t>& spriv);


    };


}