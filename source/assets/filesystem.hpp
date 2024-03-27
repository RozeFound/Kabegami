#pragma once

#include <vector>
#include <filesystem>

#include "package.hpp"
#include "utils.hpp"

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

        template <typename T = std::vector<std::byte>>
        T read (std::string_view path, std::ptrdiff_t position = 0, std::size_t size = 0) const {

            for (auto& package : packages)
                if (package.exists(path))
                    return package.read<T>(path, position, size);

            for (const auto& location : locations) {

                auto full_path = location / path;

                if (!std::filesystem::exists(full_path))
                    continue;

                return fs::read<T>(full_path, position, size);
                
            }

            throw std::runtime_error(fmt::format("Error reading file: {}", path));

        }

        template <typename T> requires std::convertible_to<std::string, T>
        std::string read (std::string_view path, std::ptrdiff_t, std::size_t) {
            auto data = read<std::vector<char>>(path);
            return std::string(data.begin(), data.end());
        }

    };

}