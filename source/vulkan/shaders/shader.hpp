#pragma once

#include "module.hpp"
#include "filesystem/vfs.hpp"

namespace vku {

    class Shader {

        std::vector<std::shared_ptr<vku::ShaderModule>> modules;
        std::shared_ptr<vk::raii::DescriptorSetLayout> descriptor_set_layout;
        
        bool read_cache (std::vector<std::vector<uint32_t>>& codes, std::filesystem::path path, XXH64_hash_t hash);
        void write_cache (std::vector<std::vector<uint32_t>>& codes, std::filesystem::path path, XXH64_hash_t hash);

        public:

        Shader() = delete;
        Shader (const fs::VFS& vfs, std::string path);

        constexpr const auto& get_modules() const { return modules; }
        constexpr const auto& get_layout() const { return descriptor_set_layout; }


    };

}