#pragma once

#include <vector>
#include <filesystem>

#include "package.hpp"

namespace assets {

    class FileSystem {

        std::vector <Package> packages;
        std::vector <std::filesystem::path> locations;

        public:

        FileSystem() = default;
        FileSystem (std::initializer_list<std::filesystem::path> locations) : locations(locations) {}

        void add_package (std::filesystem::path package) { packages.emplace_back(package); }
        void add_location (std::filesystem::path location) { locations.push_back(location); }

        bool exists (std::string_view path) const;

        template <typename T = std::vector <std::byte>> T read (std::string_view path) const;

    };

}