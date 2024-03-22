#pragma once

#include "compiler.hpp"

namespace glsl {
    
    class ShaderResources {

        struct Block {

            struct Uniform {
                uint32_t index;
                std::ptrdiff_t offset;
                std::size_t size;
            };

            std::size_t size;
            std::string name;

            std::unordered_map<std::string_view, Uniform> members;

        };

        struct Input {
            uint32_t location;
            vk::Format format;
        };

        std::vector<Block> blocks;
        std::unordered_map<std::string_view, vk::DescriptorSetLayoutBinding> binding_map;
        std::unordered_map<std::string_view, Input> input_map;

        bool reflect (const std::vector<uint32_t>& code);

        public:

        ShaderResources (const std::vector<SPV>& spvs);

        std::shared_ptr<vk::raii::DescriptorSetLayout> get_layout();

    };

}