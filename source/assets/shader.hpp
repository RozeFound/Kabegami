#pragma once

#include "filesystem.hpp"

namespace assets {

    struct ShaderUnit {
        std::string source;
        vk::ShaderStageFlagBits stage;
    };


    class ShaderParser {

        std::array<ShaderUnit, 2> units;

        public:

        ShaderParser (std::string path, const FileSystem& fs);

        constexpr auto& get_units() { return units; }

    };

}