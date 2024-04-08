#pragma once

#include "vulkan/shaders/compiler.hpp"

#include "filesystem/vfs.hpp"

namespace parsers {

    class Shader {

        std::vector<glsl::ShaderUnit> units;

        public:

        Shader (const fs::VFS& vfs, std::string path);

        constexpr const auto& get_units() const { return units; };

    };

}