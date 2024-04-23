#include "shader.hpp"

#include "reflection.hpp"
#include "parsers/shader.hpp"

namespace vku {

    constexpr auto get_hash (const std::vector<glsl::ShaderUnit>& units) {
        auto hashes = std::vector<XXH64_hash_t>();
        for (auto& unit : units) hashes.emplace_back(unit.get_hash());
        return hash::XXH3(hashes);
    }

    Shader::Shader (const fs::VFS& vfs, std::string path) {

        glsl::Compiler::Options options;

        options.auto_map_locations = true;
        options.auto_map_bindings = true;
        options.relaxed_rules_vulkan = true;
        options.relaxed_errors_glsl = true;
        options.suppress_warnings_glsl = true;

        auto compiler = glsl::Compiler(options);
        auto parser = parsers::Shader(vfs, path);
        std::vector<std::vector<uint32_t>> codes;

        auto hash = get_hash(parser.get_units());

        auto cache_mount = vfs.get_mount("cache");
        auto cache_path = path + ".spvcache";
        bool cache_read = false;

        if (cache_mount->exists(cache_path)) {
            auto file = cache_mount->open(cache_path, fs::read);
            cache_read = read_cache(codes, file, hash);
        }

        if (!cache_read) {
            if (compiler.compile(parser.get_units(), codes)) {
                auto file = cache_mount->open(cache_path, fs::write);
                write_cache(codes, file, hash);
            }
        }

        auto resources = glsl::ShaderResources(codes);
        descriptor_set_layout = resources.get_layout();

        for (auto& spv : resources.get_spvs())
            modules.emplace_back(std::make_shared<vku::ShaderModule>(spv));

    }

    bool Shader::read_cache (std::vector<std::vector<uint32_t>>& codes, fs::BinaryStream& file, XXH64_hash_t hash) {

        if (file.read<XXH64_hash_t>() != hash) return false;

        auto codes_count = file.read<uint32_t>();

        for (uint32_t i = 0; i < codes_count; i++) {

            auto size = file.read<uint32_t>();
            auto code = file.read<std::vector<uint32_t>>(size);
            codes.emplace_back(std::move(code));

        }

        return true;

    }

    void Shader::write_cache (std::vector<std::vector<uint32_t>>& codes, fs::BinaryStream& file, XXH64_hash_t hash) {

        file.write(hash);
        file.write<uint32_t>(codes.size());

        for (auto& code : codes) {
            file.write<uint32_t>(code.size());
            file.write(code);
        }

    }

}