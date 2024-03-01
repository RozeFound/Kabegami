#pragma once

#include "vulkan/core/context.hpp"

#include "assets/shader.hpp"

#include "compiler.hpp"

namespace vku {

    class ShaderModule {

        std::unique_ptr<vk::raii::ShaderModule> module;
        vk::PipelineShaderStageCreateInfo stage;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        public:

        ShaderModule (const assets::ShaderParser& parser) {

            auto stage = parser.get_stage();

            auto compiler = glsl::Compiler(stage);

            if (!compiler.compile(parser.get_source())) {
                loge("Failed to compile shader: {}", parser.get_path());
                logd("\n{}",compiler.get_log());
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