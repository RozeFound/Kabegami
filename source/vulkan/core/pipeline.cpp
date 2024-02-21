#include "pipeline.hpp"

#include "context.hpp"

#include <algorithm>

namespace vku {

    std::unique_ptr<vk::raii::Pipeline> PipeLineFactory::create (const vk::raii::PipelineCache& cache,
        const vk::raii::PipelineLayout& layout, const vk::raii::RenderPass& render_pass) {

        auto context = vki::Context::get();

        auto dynamic_states = std::vector { 
            vk::DynamicState::eViewport, 
            vk::DynamicState::eScissor
        };

        auto dynamic_state_info = vk::PipelineDynamicStateCreateInfo {
            .flags = vk::PipelineDynamicStateCreateFlags(),
            .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data()
        };

        auto viewport_state_info = vk::PipelineViewportStateCreateInfo {
            .viewportCount = 1,
            .scissorCount = 1,
        };

        auto color_blend_info = vk::PipelineColorBlendStateCreateInfo {
            .flags = vk::PipelineColorBlendStateCreateFlags(),
            .attachmentCount = 1,
            .pAttachments = &create_info.color_blend_attachment
        };

        auto stages = std::vector<vk::PipelineShaderStageCreateInfo>();
        for (const auto& shader : shaders) stages.push_back(shader.get_stage());

        auto pipeline_create_info = vk::GraphicsPipelineCreateInfo {
            .flags = vk::PipelineCreateFlags(),
            .stageCount = to_u32(stages.size()),
            .pStages = stages.data(),
            .pVertexInputState = &create_info.input_state,
            .pInputAssemblyState = &create_info.input_assembly,
            .pTessellationState = nullptr,
            .pViewportState = &viewport_state_info,
            .pRasterizationState = &create_info.rasterization,
            .pMultisampleState = &create_info.multisampling,
            .pDepthStencilState = &create_info.depth_stencil,
            .pColorBlendState = &color_blend_info,
            .pDynamicState = &dynamic_state_info,
            .layout = *layout,
            .renderPass = *render_pass,
            .subpass = 0,
            .basePipelineHandle = nullptr,
            .basePipelineIndex = -1,
        };

        try { return std::make_unique<vk::raii::Pipeline>(context->device, cache, pipeline_create_info); }
        catch (vk::SystemError e) { loge("Failed to create Graphics Pipeline: {}", e.what()); return nullptr; }

    }

    PipeLineFactory& PipeLineFactory::vertex_binding (const vk::VertexInputBindingDescription& binding) {

        create_info.input_state.vertexBindingDescriptionCount = 1;
        create_info.input_state.pVertexBindingDescriptions = &binding;

        return *this;

    }

    PipeLineFactory& PipeLineFactory::vertex_attributes (const std::vector<vk::VertexInputAttributeDescription>& attrbutes) {

        create_info.input_state.vertexAttributeDescriptionCount = to_u32(attrbutes.size());
        create_info.input_state.pVertexAttributeDescriptions = attrbutes.data();

        return *this;

    }
    
    PipeLineFactory& PipeLineFactory::stages (const std::vector<std::string_view>& shaders) {

        for (auto& shader : shaders)
            this->shaders.emplace_back(shader);

        return *this;

    }

    PipeLineFactory& PipeLineFactory::topology (vk::PrimitiveTopology topology) {

        create_info.input_assembly = vk::PipelineInputAssemblyStateCreateInfo {
            .flags = vk::PipelineInputAssemblyStateCreateFlags(),
            .topology = topology,
            .primitiveRestartEnable = false
        };

        return *this;

    }

    PipeLineFactory& PipeLineFactory::cull_mode (vk::CullModeFlagBits cull_mode) {

        create_info.rasterization = vk::PipelineRasterizationStateCreateInfo {
            .flags = vk::PipelineRasterizationStateCreateFlags(),
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = cull_mode,
            .frontFace = vk::FrontFace::eClockwise,
            .depthBiasEnable = false,
            .lineWidth = 1.f
        };

        return *this;

    }

    PipeLineFactory& PipeLineFactory::multisampling (vk::SampleCountFlagBits sample_count, bool sample_shading) {

        create_info.multisampling = vk::PipelineMultisampleStateCreateInfo {
            .flags = vk::PipelineMultisampleStateCreateFlags(),
            .rasterizationSamples = sample_count,
            .sampleShadingEnable = vk::Bool32(sample_shading)
        };

        return *this;

    }

    PipeLineFactory& PipeLineFactory::depth_stencil (bool depth_test, bool depth_write, bool stencil_test) {

        create_info.depth_stencil = vk::PipelineDepthStencilStateCreateInfo {
            .depthTestEnable = vk::Bool32(depth_test),
            .depthWriteEnable = vk::Bool32(depth_write),
            .depthCompareOp = vk::CompareOp::eLess,
            .depthBoundsTestEnable = false,
            .stencilTestEnable = vk::Bool32(stencil_test)
        };

        return *this;

    }

    PipeLineFactory& PipeLineFactory::color_blend (bool enable_blend,
        std::array<vk::BlendFactor, 2> color_blend_factor, vk::BlendOp color_blend_op,
        std::array<vk::BlendFactor, 2> alpha_blend_factor, vk::BlendOp alpha_blend_op) {

        create_info.color_blend_attachment = vk::PipelineColorBlendAttachmentState {
            .blendEnable = vk::Bool32(enable_blend),
            .srcColorBlendFactor = color_blend_factor.at(0),
            .dstColorBlendFactor = color_blend_factor.at(1),
            .colorBlendOp = color_blend_op,
            .srcAlphaBlendFactor = alpha_blend_factor.at(0),
            .dstAlphaBlendFactor = alpha_blend_factor.at(1),
            .alphaBlendOp = alpha_blend_op,
            .colorWriteMask = vk::ColorComponentFlagBits::eR 
                            | vk::ColorComponentFlagBits::eG 
                            | vk::ColorComponentFlagBits::eB 
                            | vk::ColorComponentFlagBits::eA
        };

        return *this;

    }

    std::unique_ptr<vk::raii::PipelineLayout> PipeLineLayoutFactory::create() {

        auto context = vki::Context::get();

        auto create_info = vk::PipelineLayoutCreateInfo {
            .flags = vk::PipelineLayoutCreateFlags()
        };

        if (!layouts.empty()) {
            create_info.setLayoutCount = to_u32(layouts.size());
            create_info.pSetLayouts = layouts.data();
        }

        if (!ranges.empty()) {
            create_info.pushConstantRangeCount = to_u32(ranges.size());
            create_info.pPushConstantRanges = ranges.data();
        }

        try { return std::make_unique<vk::raii::PipelineLayout>(context->device, create_info); }
        catch (vk::SystemError e) { loge("Failed to create PipeLine Layout: {}", e.what()); }
        return nullptr;

    }

    PipeLineLayoutFactory& PipeLineLayoutFactory::set_layout (const vk::DescriptorSetLayout& layout) {
        layouts.push_back(layout);
        return *this;
    }
    PipeLineLayoutFactory& PipeLineLayoutFactory::push_constant (const vk::PushConstantRange& range) {
        ranges.push_back(range);
        return *this;
    }

    PipeLineCache::PipeLineCache() {

        auto context = vki::Context::get();

        if (std::filesystem::exists(path))
            data = fs::read(path);

        auto create_info = vk::PipelineCacheCreateInfo {
            .flags = vk::PipelineCacheCreateFlags(),
            .initialDataSize = data.size(),
            .pInitialData = data.data()
        };

        handle = std::make_unique<vk::raii::PipelineCache>(context->device, create_info);

    }

    PipeLineCache::~PipeLineCache() {

        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path.parent_path());

        fs::write(path, handle->getData());

    }

}