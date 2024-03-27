#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <string>
#include <vector>

#include "utils.hpp"

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

        template <typename T = std::vector<std::byte>>
        T read (std::string_view path, std::ptrdiff_t position, std::size_t size) const {

            auto& entry = entries.at({ path.begin(), path.end() });

            file.seekg(data_offset + entry.offset);
            if (size == 0) size = entry.length;

            if constexpr (is_array_like<T>) {
                auto buffer = T();
                buffer.resize(size);
                file.read(reinterpret_cast<char*>(buffer.data()), size);
                return buffer;
            } else {
                T value;
                file.read(reinterpret_cast<char*>(&value), sizeof(T));
                return value;
            }

        }
        
        constexpr bool exists (std::string_view path) const { 
            return entries.contains({ path.begin(), path.end() });
        }

    };

}