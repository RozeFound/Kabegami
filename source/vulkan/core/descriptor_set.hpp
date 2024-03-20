#pragma once

namespace vku {

    class DescriptorSetFactory {

        std::unique_ptr<vk::raii::DescriptorPool> pool;
        std::shared_ptr<vk::raii::DescriptorSetLayout> layout;

        public:

        DescriptorSetFactory& set_layout (std::shared_ptr<vk::raii::DescriptorSetLayout> layout);

        std::shared_ptr<vk::raii::DescriptorSet> create();

    };

    class DescriptorSetLayoutFactory {

        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        public:

        std::shared_ptr<vk::raii::DescriptorSetLayout> create();

        DescriptorSetLayoutFactory& add_binding (uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlagBits stage);

    };

}