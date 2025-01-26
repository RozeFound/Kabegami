#pragma once

#include <vector>
#include <filesystem>

#include "binary_stream.hpp"
#include "location.hpp"
#include "package.hpp"

namespace fs {

    class VFS {

        std::vector<std::shared_ptr<Mount>> mounts;

        public:

        VFS() = default;
        VFS (std::initializer_list<std::filesystem::path> locations) {
            for (const auto& location : locations) mount_location(location);
        }

        void mount_package (std::filesystem::path path, std::string name = "") {
            mounts.emplace_back(std::make_shared<Package>(path, name));
        }

        void mount_location (std::filesystem::path path, std::string name = "") {
            mounts.emplace_back(std::make_shared<Location>(path, name));
        }

        constexpr bool is_mounted (std::string_view name) const {
            for (const auto& mount : mounts)
                if (mount->get_name() == name) return true;
            return false; 
        }

        std::shared_ptr<Mount> get_mount (std::string_view name) const {
            for (const auto& mount : mounts)
                if (mount->get_name() == name) return mount;
            throw std::runtime_error(fmt::format("Failed to get mount: {}", name));
        }

        bool exists (std::filesystem::path path) const {
            for (const auto& mount : mounts)
                if (mount->exists(path)) return true;
            return false;
        }

        BinaryStream open (std::filesystem::path path, access_flags mode) const {
            for (const auto& mount : mounts)
                if (mount->exists(path)) return mount->open(path, mode);
            throw std::runtime_error(fmt::format("Failed to open file: {}", path.string())); 
        }

        template <typename T = std::vector<std::byte>>
        T read (std::filesystem::path path, std::size_t size = 0, std::ptrdiff_t offset = 0) const {
            return open(path, fs::read).read<T>(size, offset);
        }

        template <typename T = std::vector<std::byte>>
        void write (std::filesystem::path path, T data) const {
            open(path, fs::write).write<T>(data);
        }

    };

}