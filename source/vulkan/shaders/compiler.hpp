#pragma once

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>

#include "utils.hpp"

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
        
        struct PreProcessInfo {
            std::unordered_map<std::string, std::string> inputs;
            std::unordered_map<std::string, std::string> outputs;
        } preprocess_info;

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

        constexpr const auto& get_preamble() const { return preamble; }
        constexpr const auto& get_processes() const { return processes; }

        constexpr const auto get_hash() const { return hash::XXH3(source); }

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

        Compiler() : Compiler(Options{}) {} ;
        Compiler (Options options) : options(options) { glslang::InitializeProcess(); set_messages(); };
        ~Compiler() { glslang::FinalizeProcess(); };

        bool parse (const ShaderUnit& unit, glslang::TShader& shader);
        bool compile (const std::vector<ShaderUnit>& units, std::vector<std::vector<uint32_t>>& codes);


    };


}