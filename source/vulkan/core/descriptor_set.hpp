#pragma once

namespace vku {

    class DescriptorSetLayoutFactory {

        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        public:

        std::unique_ptr<vk::raii::DescriptorSetLayout> create();

        DescriptorSetLayoutFactory& add_binding (uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlagBits stage);

    };

}