#pragma once

#include <vector>
#include <filesystem>

#include "binary_stream.hpp"
#include "package.hpp"

namespace fs {

    class VFS {

        std::vector<Package> packages;
        std::vector<std::filesystem::path> locations;

        public:

        VFS() = default;
        VFS (std::initializer_list<std::filesystem::path> locations) : locations(locations) {}

        void add_package (std::filesystem::path package) { packages.emplace_back(package); }
        void add_location (std::filesystem::path location) { locations.push_back(location); }

        constexpr bool exists (std::string_view path) const {

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

        template <typename T = std::vector<std::byte>>
        T read (std::string_view path, std::size_t size = 0, std::ptrdiff_t offset = 0) const {

            for (auto& package : packages)
                if (package.exists(path))
                    return package.read<T>(path, size);

            for (const auto& location : locations) {

                auto full_path = location / path;

                if (std::filesystem::exists(full_path)) {
                    auto file = BinaryStream(full_path, fs::read);
                    return file.read<T>(size, offset);
                }

            }

            throw std::runtime_error(fmt::format("Error reading file: {}", path));

        }

    };

}