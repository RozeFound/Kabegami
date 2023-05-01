#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <string>
#include <vector>

struct Entry {
    uint32_t offset;
    uint32_t length;
};

class Package {

    mutable std::ifstream file;

    std::ptrdiff_t data_offset;

    std::unordered_map <std::string, Entry> entries;

    uint32_t read_int();
    std::string read_string();

    public:

    Package (std::filesystem::path path);
    Package (Package&&) noexcept = default;
    Package (Package&) = delete;

    std::vector <std::byte> read_file (std::string_view path) const;
    constexpr bool exists (std::string_view path) const { 
        return entries.contains({ path.begin(), path.end() });
    }

};