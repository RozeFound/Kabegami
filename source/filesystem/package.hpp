#pragma once

#include <cstdint>
#include <unordered_map>
#include <filesystem>
#include <string_view>
#include <string>
#include <vector>

#include "mount.hpp"
#include "binary_stream.hpp"

namespace fs {

    struct Entry {
        uint32_t offset;
        uint32_t length;
    };

    class Package : public Mount {

        std::unique_ptr<BinaryStream> file;

        std::ptrdiff_t data_offset;

        std::unordered_map<std::string, Entry> entries;

        public:

        Package (std::filesystem::path path, std::string_view name)
            : Mount(path, name) {

            file = std::make_unique<FileBinaryStream>(path, fs::read);

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

        
        Package (Package&& other) noexcept;
        Package (Package&) = delete;

        BinaryStream open (std::filesystem::path path, access_flags mode) const {

            if (mode == fs::write) throw std::runtime_error("Package is read only");

            auto& entry = entries.at(path);

            auto size = entry.length;
            auto offset = data_offset + entry.offset;

            using T = std::vector<std::byte>;
            auto data = file->read<T>(size, offset);
            return MemoryBinaryStream(std::move(data), fs::read);

        }
        
        bool exists (std::filesystem::path path) const { 
            return entries.contains(path);
        }

    };

}