#pragma once

#include <glaze/glaze.hpp>

namespace objects {

    struct Model {
        bool autosize;
        bool fullscreen;
        bool passthrough;
        std::string_view material;
        std::string_view puppet;
    };

}