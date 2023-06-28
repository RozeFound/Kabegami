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

template <> struct glz::meta<objects::Model> {

    using T = objects::Model;

    static constexpr auto value = glz::object ( 

        "autosize", &T::autosize,
        "fullscreen", &T::fullscreen,
        "passthrough", &T::passthrough,
        "material", &T::material,
        "puppet", &T::puppet

    );

};