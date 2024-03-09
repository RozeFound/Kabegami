#pragma once

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

namespace glsl {

    constexpr auto find_shader_language (vk::ShaderStageFlagBits stage) {
        switch (stage)
        {
            case vk::ShaderStageFlagBits::eVertex: return EShLangVertex;
            case vk::ShaderStageFlagBits::eFragment: return EShLangFragment;
            case vk::ShaderStageFlagBits::eGeometry: return EShLangGeometry;
            default: return EShLangVertex;
        }
    }

    class ShaderUnit {

        std::string preamble;
        std::vector<std::string> processes;

        public:

        std::string source;
        const vk::ShaderStageFlagBits stage;
        const EShLanguage language = find_shader_language(stage);

        ShaderUnit(const std::string& source, vk::ShaderStageFlagBits stage) : source(source), stage(stage) {}

        inline void define (const std::string& name) {
            preamble += "#define " + name + "\n";
            processes.emplace_back("D" + name);
        }

        inline void define (const std::string& name, const std::string& value) {
            preamble += "#define " + name + " " + value + "\n";
            processes.emplace_back("D" + name + "=" + value);
        }
        
        inline void undefine (const std::string& name) {
            preamble += "#undef " + name + "\n";
            processes.emplace_back("U" + name);
        }

        constexpr const auto get_preamble() const { return preamble; }
        constexpr const auto get_processes() const { return processes; }

    };

    class Compiler {

        void set_messages();

        const TBuiltInResource* resource_limits = GetDefaultResources();
        EShMessages messages;
        
        constexpr glslang::EShClient get_client (glslang::EShTargetClientVersion version);
        constexpr glslang::EShTargetLanguageVersion get_target_version (glslang::EShTargetClientVersion version);

        public:

        struct Options {

            glslang::EShTargetClientVersion client_version = glslang::EShTargetVulkan_1_3;

            bool hlsl = false;

            bool auto_map_locations = false;
            bool auto_map_bindings = false;

            bool relaxed_rules_vulkan = false;
            int global_uniform_binding = 0;

            bool relaxed_errors_glsl = false;
            bool suppress_warnings_glsl = false;

            bool optimize = false;
            bool optimize_size = false;
            
        } const options;

        Compiler() { glslang::InitializeProcess(); set_messages(); };
        Compiler (Options options) : Compiler() { options = options; };
        ~Compiler() { glslang::FinalizeProcess(); };

        bool parse (ShaderUnit& unit, glslang::TShader& shader);
        bool compile (ShaderUnit& unit, std::vector<uint32_t>& spriv);


    };


}