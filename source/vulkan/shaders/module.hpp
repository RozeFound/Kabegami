#pragma once

#include "vulkan/core/context.hpp"
#include "compiler.hpp"

namespace vku {

    class ShaderModule {

        std::unique_ptr<vk::raii::ShaderModule> module;
        vk::PipelineShaderStageCreateInfo stage;

        std::shared_ptr<vki::Context> context = vki::Context::get();

        public:

        ShaderModule (const glsl::SPV& spv) {

            auto create_info = vk::ShaderModuleCreateInfo {
                .flags = vk::ShaderModuleCreateFlags(),
                .codeSize = spv.size,
                .pCode = spv.code.data()
            };

            try { this->module = std::make_unique<vk::raii::ShaderModule>(context->device, create_info); }
            catch (vk::SystemError e) { loge("Failed to create shader module: {}", e.what()); return; };

            this->stage = vk::PipelineShaderStageCreateInfo {
                .flags = vk::PipelineShaderStageCreateFlags(),
                .stage = spv.stage,
                .module = **module,
                .pName = "main"
            };

        }

        constexpr const auto& get_stage() const { return stage; }

    };

}