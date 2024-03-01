#pragma once

#include "filesystem.hpp"

namespace assets {

    struct ShaderUnit {
        std::string src;
        vk::ShaderStageFlagBits stage;
    };

    class ShaderParser {

        vk::ShaderStageFlagBits stage;
        std::string_view path;
        std::string source;

        public:

        ShaderParser (std::string_view path, const FileSystem& fs);

        constexpr const auto& get_stage() const { return stage; }
        constexpr const auto& get_source() const { return source; }
        constexpr const auto& get_path() const { return path; }

    };

}