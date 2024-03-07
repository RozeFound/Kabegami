#pragma once

#include "vulkan/shaders/compiler.hpp"

#include "filesystem.hpp"

namespace assets {

    class ShaderParser {

        std::vector<glsl::ShaderUnit> units;

        std::string pre_header (glsl::ShaderUnit& unit);

        public:

        ShaderParser (std::string path, const FileSystem& fs);

        constexpr auto& get_units() { return units; }

    };

}