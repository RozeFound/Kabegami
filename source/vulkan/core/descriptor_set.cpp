#include "descriptor_set.hpp"

#include "context.hpp"

namespace vku {

    DescriptorSetFactory& DescriptorSetFactory::set_layout (std::shared_ptr<vk::raii::DescriptorSetLayout> layout) {

        this->layout = layout;

        return *this;

    }

    std::shared_ptr<vk::raii::DescriptorSet> DescriptorSetFactory::create() {

        auto context = vki::Context::get();

        auto pool_size = vk::DescriptorPoolSize {
            .type = vk::DescriptorType::eCombinedImageSampler,
            .descriptorCount = 1
        };

        auto pool_info = vk::DescriptorPoolCreateInfo {
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = 1,
            .poolSizeCount = 1,
            .pPoolSizes = &pool_size
        };

        try { pool = std::make_unique<vk::raii::DescriptorPool>(context->device, pool_info); }
        catch (vk::SystemError e) { loge("Failed to create Descriptor Pool: {}", e.what()); }

        auto allocate_info = vk::DescriptorSetAllocateInfo {
            .descriptorPool = **pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &**layout
        };

        try { auto sets = vk::raii::DescriptorSets(context->device, allocate_info);
              return std::make_shared<vk::raii::DescriptorSet>(std::move(sets.back()));
        } catch (vk::SystemError e) { loge("Failed to allocate DescriptorSet: {}", e.what()); }

        return nullptr;

    }
    
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

    std::shared_ptr<vk::raii::DescriptorSetLayout> DescriptorSetLayoutFactory::create() {

        auto context = vki::Context::get();

        auto create_info = vk::DescriptorSetLayoutCreateInfo {
            .flags = vk::DescriptorSetLayoutCreateFlags(),
            .bindingCount = to_u32(bindings.size()),
            .pBindings = bindings.data()
        };

        try { return std::make_shared<vk::raii::DescriptorSetLayout>(context->device, create_info); }
        catch (vk::SystemError error) { loge("Failed to create DescriptorSet layout"); return nullptr; }

    }

}