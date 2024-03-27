#pragma once

namespace glsl {

    struct SPV {
        std::vector<uint32_t> code;
        std::uint32_t size;
        vk::ShaderStageFlagBits stage;
    };
    
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

        std::shared_ptr<vk::raii::DescriptorSetLayout> layout;
        std::vector<SPV> spvs;


        bool reflect (const std::vector<uint32_t>& code);

        public:

        ShaderResources (std::vector<std::vector<uint32_t>>& codes);

        constexpr auto& get_layout () const { return layout; }
        constexpr auto& get_spvs () const { return spvs; }

    };

}