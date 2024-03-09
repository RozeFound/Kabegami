#pragma once

#include "vulkan/shaders/compiler.hpp"

#include "filesystem.hpp"

namespace assets {

    struct Parameter {
        std::string type;
        std::string name;
        union {
            uint32_t location;
            uint32_t binding;
        };
    };

    class ShaderParser {

        std::vector<glsl::ShaderUnit> units;

        public:

        ShaderParser (const FileSystem& fs, std::string path);

        constexpr auto& get_units() { return units; }

    };

}