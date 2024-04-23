#pragma once

#include <filesystem>

#include "binary_stream.hpp"

namespace fs {

    class Mount {

        std::string name;
        std::filesystem::path path;

        public:

        Mount() = delete;
        Mount (std::filesystem::path path, std::string_view name)
            : path(path), name(name) {}

        Mount (Mount&&) = default;
        Mount (Mount&) = delete;

        virtual ~Mount() = default;

        virtual BinaryStream open (std::filesystem::path path, access_flags mode) const = 0;
        virtual bool exists (std::filesystem::path path) const = 0;

        virtual std::string_view get_name() const { return name; }
        virtual std::filesystem::path get_path() const { return path; }

    };

}