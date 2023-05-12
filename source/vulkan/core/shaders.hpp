#pragma once

#include "context.hpp"

namespace vku {

    class Shader {

        std::unique_ptr<vk::raii::ShaderModule> module;
        vk::PipelineShaderStageCreateInfo stage;

        std::shared_ptr <vki::Context> context = vki::Context::get();

        public:

        Shader (std::convertible_to <std::filesystem::path> auto path) {

            auto shader_path = std::filesystem::path(path);
            auto stage = vk::ShaderStageFlagBits::eVertex;

            if (shader_path.extension() == ".vert")
                stage = vk::ShaderStageFlagBits::eVertex;
            if (shader_path.extension() == ".frag")
                stage = vk::ShaderStageFlagBits::eFragment;
            if (shader_path.extension() == ".comp")
                stage = vk::ShaderStageFlagBits::eCompute;

            auto buffer = vku::fs::read(shader_path);

            auto create_info = vk::ShaderModuleCreateInfo {
                .flags = vk::ShaderModuleCreateFlags(),
                .codeSize = buffer.size(),
                .pCode = reinterpret_cast<const uint32_t*>(buffer.data())
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