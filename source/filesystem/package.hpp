#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <string_view>
#include <string>
#include <vector>

#include "binary_stream.hpp"

namespace fs {

    struct Entry {
        uint32_t offset;
        uint32_t length;
    };

    class Package {

        std::unique_ptr<BinaryStream> file;

        std::ptrdiff_t data_offset;

        std::unordered_map<std::string, Entry> entries;

        public:

        Package (std::filesystem::path path) {

            file = std::make_unique<BinaryStream>(path, fs::read);

            auto package_version = file->read<std::string>(file->read<uint32_t>());
            logi("Package version: {}", package_version);

            auto entries_count = file->read<uint32_t>();

            for (std::size_t i = 0; i < entries_count; i++) {

                auto entry_name = file->read<std::string>(file->read<uint32_t>());
                
                entries[entry_name].offset = file->read<uint32_t>();
                entries[entry_name].length = file->read<uint32_t>();

            }

            data_offset = file->get_position();

        }

        Package (Package&&) noexcept = default;
        Package (Package&) = delete;

        template <typename T = std::vector<std::byte>>
        T read (std::string_view path, std::size_t size) const {

            auto& entry = entries.at({ path.begin(), path.end() });
            if (size == 0) size = entry.length;
            auto offset = data_offset + entry.offset;
            return file->read<T>(size, offset);

        }
        
        constexpr bool exists (std::string_view path) const { 
            return entries.contains({ path.begin(), path.end() });
        }

    };

}