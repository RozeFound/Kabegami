#pragma once

#include "vulkan/core/context.hpp"

#include "compiler.hpp"

namespace vku {

    class ShaderModule {

        std::unique_ptr<vk::raii::ShaderModule> module;
        vk::PipelineShaderStageCreateInfo stage;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        public:

        ShaderModule (std::convertible_to<std::filesystem::path> auto path, const assets::FileSystem& fs) {

            auto shader_path = std::filesystem::path(path);
            auto stage = vk::ShaderStageFlagBits::eVertex;

            if (shader_path.extension() == ".vert")
                stage = vk::ShaderStageFlagBits::eVertex;
            if (shader_path.extension() == ".frag")
                stage = vk::ShaderStageFlagBits::eFragment;
            if (shader_path.extension() == ".comp")
                stage = vk::ShaderStageFlagBits::eCompute;

            auto glsl_source = fs.read<uint8_t>(path);
            auto compiler = glsl::Compiler(stage, fs);

            if (!compiler.compile(glsl_source)) {
                loge("Failed to compile shader: {}", shader_path.string());
                logv("\n{}",compiler.get_logs());
            }

            auto buffer = compiler.get_spirv();

            auto create_info = vk::ShaderModuleCreateInfo {
                .flags = vk::ShaderModuleCreateFlags(),
                .codeSize = buffer.size() * sizeof(uint32_t),
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