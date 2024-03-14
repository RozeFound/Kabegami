#pragma once

#include "vulkan/shaders/compiler.hpp"

#include "filesystem.hpp"

namespace assets {

    class ShaderParser {

        std::vector<glsl::ShaderUnit> units;

        public:

        ShaderParser (const FileSystem& fs, std::string path);

        constexpr const auto& get_units() const { return units; };

    };

}