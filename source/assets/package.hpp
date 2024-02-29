#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <string>
#include <vector>

namespace assets {

    struct Entry {
        uint32_t offset;
        uint32_t length;
    };

    class Package {

        mutable std::ifstream file;

        std::ptrdiff_t data_offset;

        std::unordered_map<std::string, Entry> entries;

        uint32_t read_int();
        std::string read_string();

        public:

        Package (std::filesystem::path path);
        Package (Package&&) noexcept = default;
        Package (Package&) = delete;

        template<typename T> std::vector<T> read_file (std::string_view path) const {

            auto& entry = entries.at({ path.begin(), path.end() });
            auto result = std::vector<T>(entry.length / sizeof(T));

            file.seekg(data_offset + entry.offset);

            file.read(reinterpret_cast<char*>(result.data()), entry.length);
            
            return result;

        }
        
        constexpr bool exists (std::string_view path) const { 
            return entries.contains({ path.begin(), path.end() });
        }

    };

}