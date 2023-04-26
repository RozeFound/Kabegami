#include "filesystem.hpp"

#include <fmt/core.h>

bool FileSystem::exists (std::string_view path) {

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

        auto file = std::ifstream(full_path, std::ios::binary | std::ios::ate);
        
        auto size = file.tellg(); file.seekg(0);
        auto result = std::vector<std::byte>(size); 

        file.read(reinterpret_cast<char*>(result.data()), size);
        return result;
        
    }

    throw std::runtime_error(fmt::format("Error reading file: {}", path));

}

template <> std::string FileSystem::read (std::string_view path) const {
    return reinterpret_cast<char*>(read(path).data());
}