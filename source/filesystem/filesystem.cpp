#include "filesystem.hpp"

#include <fmt/core.h>

std::vector<std::byte> FileSystem::read_file (std::string_view path) {

    for (auto& package : packages) {

        if (package.exists(path))
            return package.read_file(path);

    }
        
    for (const auto& location : locations) {

        auto full_path = location / path;
        
        auto file = std::ifstream(full_path, std::ios::binary | std::ios::ate);
        
        auto size = file.tellg(); file.seekg(0);
        auto result = std::vector<std::byte>(size); 

        file.read(reinterpret_cast<char*>(result.data()), size);
        return result;
        
    }

    throw std::runtime_error(fmt::format("Can't find file {}", path));

}