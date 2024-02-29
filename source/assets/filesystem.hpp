#pragma once

#include <vector>
#include <filesystem>

#include "package.hpp"
#include "vulkan/utility/misc.hpp"

namespace assets {

    class FileSystem {

        std::vector<Package> packages;
        std::vector<std::filesystem::path> locations;

        public:

        FileSystem() = default;
        FileSystem (std::initializer_list<std::filesystem::path> locations) : locations(locations) {}

        void add_package (std::filesystem::path package) { packages.emplace_back(package); }
        void add_location (std::filesystem::path location) { locations.push_back(location); }

        bool exists (std::string_view path) const;

        template <typename T = std::byte> std::vector<T> read (std::string_view path) const {

            for (auto& package : packages)
                if (package.exists(path))
                    return package.read_file<T>(path);

            for (const auto& location : locations) {

                auto full_path = location / path;

                if (!std::filesystem::exists(full_path))
                    continue;

                return vku::fs::read<T>(full_path);
                
            }

            throw std::runtime_error(fmt::format("Error reading file: {}", path));

        }

        const std::string read_as_string(std::string_view path) const;

    };

}