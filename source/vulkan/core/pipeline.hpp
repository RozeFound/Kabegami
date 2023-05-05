#pragma once

#include "shaders.hpp"

namespace vku {

    class PipeLineFactory {



        struct {
            vk::PipelineVertexInputStateCreateInfo input_state;
            vk::PipelineInputAssemblyStateCreateInfo input_assembly;
            vk::PipelineRasterizationStateCreateInfo rasterization;
            vk::PipelineMultisampleStateCreateInfo multisampling;
            vk::PipelineDepthStencilStateCreateInfo depth_stencil;
            vk::PipelineColorBlendAttachmentState color_blend_attachment;
        } create_info;
        

        std::vector <Shader> shaders;

        public:

        PipeLineFactory() {

            color_blend(false, {}, {}, {}, {});
            topology(vk::PrimitiveTopology::eTriangleList);
            cull_mode(vk::CullModeFlagBits::eBack);
            multisampling(vk::SampleCountFlagBits::e1, false);
            depth_stencil(false, false, false);

        }

        std::unique_ptr <vk::raii::Pipeline> create (const vk::raii::PipelineCache& cache, 
            const vk::raii::PipelineLayout& layout, const vk::raii::RenderPass& render_pass);

        PipeLineFactory& vertex_binding (const vk::VertexInputBindingDescription& description);
        PipeLineFactory& vertex_attributes (const std::vector<vk::VertexInputAttributeDescription>& description);

        PipeLineFactory& stage (Shader shader);
        PipeLineFactory& topology (vk::PrimitiveTopology topology);
        PipeLineFactory& cull_mode (vk::CullModeFlagBits cull_mode);
        PipeLineFactory& multisampling (vk::SampleCountFlagBits sample_count, bool sample_shading);
        PipeLineFactory& depth_stencil (bool depth_test, bool depth_write, bool stencil_test);
        PipeLineFactory& color_blend (bool enable_blend,
            std::array <vk::BlendFactor, 2> color_blend_factor, vk::BlendOp color_blend_op,
            std::array <vk::BlendFactor, 2> alpha_blend_factor, vk::BlendOp alpha_blend_op);

    };

    class PipeLineLayoutFactory {

        std::vector <vk::DescriptorSetLayout> layouts;
        std::vector <vk::PushConstantRange> ranges;

        public:

        std::unique_ptr<vk::raii::PipelineLayout> create();

        PipeLineLayoutFactory& set_layout (const vk::DescriptorSetLayout& layout);
        PipeLineLayoutFactory& push_constant (const vk::PushConstantRange& range);

    };

    class PipeLineCache {

        std::unique_ptr <vk::raii::PipelineCache> handle;
        std::vector <std::byte> data;

        public:

        PipeLineCache();
        ~PipeLineCache();

        constexpr const auto& operator*() const { return *handle; };

    };

}
