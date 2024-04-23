#pragma once

#include "mount.hpp"
#include "binary_stream.hpp"

namespace fs {

    class Location : public Mount {

        public:

        Location (std::filesystem::path path, std::string_view name)
            : Mount(path, name) {}

        Location (Location&&) = default;
        Location (Location&) = delete;

        BinaryStream open (std::filesystem::path path, access_flags mode) const {
            auto full_path = get_path() / path;

            if (mode == fs::write && !std::filesystem::exists(full_path.parent_path()))
                std::filesystem::create_directories(full_path.parent_path());

            return FileBinaryStream(full_path, mode);
        }

        bool exists (std::filesystem::path path) const {
            auto full_path = get_path() / path;
            return std::filesystem::exists(full_path);
        }

    };

}