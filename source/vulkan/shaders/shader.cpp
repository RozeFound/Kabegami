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

        auto hash = get_hash(parser.get_units());

        std::vector<std::vector<uint32_t>> codes;
        auto cache_path = fs::get_cache_dir() / (path + ".spvcache");

        if (!read_cache(codes, cache_path, hash)) {
            if (!compiler.compile(parser.get_units(), codes))
                loge("Failed to compile shader: {}", path);
            else write_cache(codes, cache_path, hash);
        }

        auto resources = glsl::ShaderResources(codes);
        descriptor_set_layout = resources.get_layout();

        for (auto& spv : resources.get_spvs())
            modules.emplace_back(std::make_shared<vku::ShaderModule>(spv));

    }

    bool Shader::read_cache (std::vector<std::vector<uint32_t>>& codes, std::filesystem::path path, XXH64_hash_t hash) {

        if (!std::filesystem::exists(path)) return false;

        auto file = fs::BinaryStream(path, fs::read);

        if (file.read<XXH64_hash_t>() != hash) return false;

        auto codes_count = file.read<uint32_t>();

        for (uint32_t i = 0; i < codes_count; i++) {

            auto size = file.read<uint32_t>();
            auto code = file.read<std::vector<uint32_t>>(size);
            codes.emplace_back(std::move(code));

        }

        return true;

    }

    void Shader::write_cache (std::vector<std::vector<uint32_t>>& codes, std::filesystem::path path, XXH64_hash_t hash) {

        if (!std::filesystem::exists(path))
            std::filesystem::create_directories(path.parent_path());

        auto file = fs::BinaryStream(path, fs::write);

        file.write(hash);
        file.write<uint32_t>(codes.size());

        for (auto& code : codes) {
            file.write<uint32_t>(code.size());
            file.write(code);
        }

    }

}