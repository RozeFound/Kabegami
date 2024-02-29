#include "filesystem.hpp"

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

    const std::string FileSystem::read_as_string (std::string_view path) const {
        return reinterpret_cast<char*>(read(path).data());
    }

}