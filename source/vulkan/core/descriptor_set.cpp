#include "descriptor_set.hpp"

#include "context.hpp"

namespace vku {
    
    DescriptorSetLayoutFactory& DescriptorSetLayoutFactory::add_binding (uint32_t binding, vk::DescriptorType type, vk::ShaderStageFlagBits stage) {

        bindings.push_back(
            vk::DescriptorSetLayoutBinding {
                .binding = binding,
                .descriptorType = type,
                .descriptorCount = 1,
                .stageFlags = stage
            }
        );

        return *this;

    }

    std::unique_ptr<vk::raii::DescriptorSetLayout> DescriptorSetLayoutFactory::create() {

        auto context = vki::Context::get();

        auto create_info = vk::DescriptorSetLayoutCreateInfo {
            .flags = vk::DescriptorSetLayoutCreateFlags(),
            .bindingCount = to_u32(bindings.size()),
            .pBindings = bindings.data()
        };

        try { return std::make_unique<vk::raii::DescriptorSetLayout>(context->device, create_info); }
        catch (vk::SystemError error) { loge("Failed to create DescriptorSet layout"); return nullptr; }

    }

}