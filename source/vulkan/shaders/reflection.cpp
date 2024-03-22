#include "reflection.hpp"

#include <spirv_reflect.h>

namespace glsl {

    // taken from https://github.com/catsout/wallpaper-engine-kde-plugin/blob/34f7f01acba3bc8f94d478032cf86aef06b02d26/src/backend_scene/src/Vulkan/Shader.cpp#L55
    bool EnumAllRef (auto& vec, auto&& func) {

        uint32_t count = 0;
        auto result = func(&count, nullptr);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        vec.resize(count);
        result = func(&count, vec.data());
        assert(result == SPV_REFLECT_RESULT_SUCCESS);

        return result == SPV_REFLECT_RESULT_SUCCESS;

    }

    vk::Format toVkType (SpvReflectFormat type) { return static_cast<vk::Format>(type); }

    vk::ShaderStageFlagBits toVkType (SpvReflectShaderStageFlagBits stage) {
        switch (stage) {
            case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
                return vk::ShaderStageFlagBits::eVertex;
            case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
                return vk::ShaderStageFlagBits::eGeometry;
            case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
                return vk::ShaderStageFlagBits::eFragment;
            case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
                return vk::ShaderStageFlagBits::eCompute;
            default: return vk::ShaderStageFlagBits::eAll;
        }
    }

    ShaderResources::ShaderResources (const std::vector<SPV>& spvs) {

        for (const auto& spv : spvs)
            reflect(spv.code);

    }

    bool ShaderResources::reflect (const std::vector<uint32_t>& code) {

        auto spv_ref = spv_reflect::ShaderModule(code, SPV_REFLECT_MODULE_FLAG_NO_COPY);
        auto stage = toVkType(spv_ref.GetShaderStage());

        std::vector<SpvReflectInterfaceVariable*> inputs;
        std::vector<SpvReflectDescriptorBinding*> bindings;

        if (!EnumAllRef(bindings, [&](auto&&... args) {
            return spv_ref.EnumerateDescriptorBindings(args...);
        })) return false;

        vk::DescriptorSetLayoutBinding vk_binding;
        vk_binding.stageFlags = stage;

        for (auto binding : bindings) {

            auto& bind_name = binding->type_description->type_name == nullptr ?
                              binding->name : binding->type_description->type_name;

            if (binding_map.contains(bind_name)) {
                auto& bind = binding_map[bind_name];
                bind.stageFlags |= stage; continue; 
            }

            if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {

                vk_binding.binding = binding->binding;
                vk_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
                vk_binding.descriptorCount = 1;

                auto& block = binding->block;
                auto& block_name = std::string(block.name).empty() ? bind_name : block.name;

                blocks.push_back(Block{
                    .size = block.size, 
                    .name = block_name
                });

                for (uint32_t i = 0; i < block.member_count; i++) {

                    auto& uniform = block.members[i];
                    blocks.back().members[uniform.name] = {
                        .index = i,
                        .offset = uniform.offset,
                        .size = uniform.size
                    };
                    
                }
            } else if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {

                vk_binding.binding = binding->binding;
                vk_binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
                vk_binding.descriptorCount = 1;

            } else {
                loge("Unknown binding {}", uint32_t(binding->descriptor_type));
                return false;
            }

            binding_map[bind_name] = vk_binding;

        }

        if (stage == vk::ShaderStageFlagBits::eVertex) {

            if (!EnumAllRef(inputs, [&](auto&&... args) {
                return spv_ref.EnumerateInputVariables(args...);
            })) return false;

            for (auto input : inputs) {

                auto& input_name = input->type_description->type_name == nullptr ?
                                   input->name : input->type_description->type_name;

                if (std::string(input_name).starts_with("gl_"))
                    continue;

                if (input->location == std::numeric_limits<decltype(input->location)>::max()) {
                    loge("Vertex input {} has no location", input_name);
                    return false;
                }

                input_map[input_name] = {
                    .location = input->location,
                    .format = toVkType(input->format)
                };
                
            }

        }

        return true;

    }

    std::shared_ptr<vk::raii::DescriptorSetLayout> ShaderResources::get_layout() {

        loge("glsl::ShaderResources::get_layout() not implemented");
        return std::nullptr_t{};

    }



}