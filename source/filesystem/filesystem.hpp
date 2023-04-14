#pragma once

#include <vector>
#include <filesystem>

#include "package.hpp"

class FileSystem {

    std::vector <Package> packages;
    std::vector <std::filesystem::path> locations;

    public:

    void add_package (std::filesystem::path package) { packages.emplace_back(package); }
    void add_location (std::filesystem::path location) { locations.push_back(location); }

    bool exists (std::string_view path);

    std::vector <std::byte> read_file (std::string_view path);
    std::string read_as_string (std::string_view path);

};