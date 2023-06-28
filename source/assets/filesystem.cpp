#include "filesystem.hpp"

#include "vulkan/utility/misc.hpp"

#include <fmt/core.h>

namespace assets {

    bool FileSystem::exists (std::string_view path) const {

        for (const auto& package : packages)
            if (package.exists(path))
                return true;

        for (const auto& location : locations) {
            auto full_path = location / path;
            if (std::filesystem::exists(full_path))
                return true;
        }

        return false;
        
    }

    template <> std::vector<std::byte> FileSystem::read (std::string_view path) const {

        for (auto& package : packages)
            if (package.exists(path))
                return package.read_file(path);

        for (const auto& location : locations) {

            auto full_path = location / path;

            if (!std::filesystem::exists(full_path))
                continue;

            return vku::fs::read(full_path);
            
        }

        throw std::runtime_error(fmt::format("Error reading file: {}", path));

    }

    template <> std::string FileSystem::read (std::string_view path) const {
        return reinterpret_cast<char*>(read(path).data());
    }

}