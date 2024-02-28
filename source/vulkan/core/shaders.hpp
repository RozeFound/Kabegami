#pragma once

#include <shaderc/shaderc.hpp>

#include "context.hpp"

namespace vku {

    namespace glsl {

        constexpr shaderc_shader_kind find_shader_language (vk::ShaderStageFlagBits stage) {

            switch (stage) {
                case vk::ShaderStageFlagBits::eVertex: return shaderc_glsl_vertex_shader;
                case vk::ShaderStageFlagBits::eFragment: return shaderc_glsl_fragment_shader;
                case vk::ShaderStageFlagBits::eCompute: return shaderc_glsl_compute_shader;
                default: return shaderc_glsl_infer_from_source;
            }

        }

        inline std::vector<uint32_t> compile (vk::ShaderStageFlagBits stage, std::vector<char> source) {

            shaderc::Compiler compiler;
            shaderc::CompileOptions options;

            options.SetTargetEnvironment(shaderc_target_env_default,
                                        shaderc_env_version_vulkan_1_3);
            options.SetTargetSpirv(shaderc_spirv_version_1_6);
            options.SetGenerateDebugInfo();

            auto kind = find_shader_language(stage);

            auto result = compiler.PreprocessGlsl({source.cbegin(), source.cend()} , kind, "main", options);

            if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
                loge(result.GetErrorMessage());
                return std::vector<uint32_t>();
            }

            auto module = compiler.CompileGlslToSpv({result.cbegin(), result.cend()}, kind, "main", options);

            if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                loge(result.GetErrorMessage());
                return std::vector<uint32_t>();
            }

            return { module.cbegin(), module.cend() };

        }

    }

    class Shader {

        std::unique_ptr<vk::raii::ShaderModule> module;
        vk::PipelineShaderStageCreateInfo stage;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        public:

        Shader (std::convertible_to<std::filesystem::path> auto path) {

            auto shader_path = std::filesystem::path(path);
            auto stage = vk::ShaderStageFlagBits::eVertex;

            if (shader_path.extension() == ".vert")
                stage = vk::ShaderStageFlagBits::eVertex;
            if (shader_path.extension() == ".frag")
                stage = vk::ShaderStageFlagBits::eFragment;
            if (shader_path.extension() == ".comp")
                stage = vk::ShaderStageFlagBits::eCompute;

            auto glsl_source = vku::fs::read<char>(shader_path);
            auto buffer = vku::glsl::compile(stage, glsl_source);

            auto create_info = vk::ShaderModuleCreateInfo {
                .flags = vk::ShaderModuleCreateFlags(),
                .codeSize = buffer.size(),
                .pCode = buffer.data()
            };

            try { this->module = std::make_unique<vk::raii::ShaderModule>(context->device, create_info); }
            catch (vk::SystemError e) { loge("Failed to create shader module: {}", e.what()); return; };

            this->stage = vk::PipelineShaderStageCreateInfo {
                .flags = vk::PipelineShaderStageCreateFlags(),
                .stage = stage,
                .module = **module,
                .pName = "main"
            };

        }

        constexpr const auto& get_stage() const { return stage; }

    };

}