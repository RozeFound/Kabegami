#pragma once

#include "vectors.hpp"

namespace objects {

    struct Light {

        bool visible;
        std::string_view name;

        std::string_view light;
        double intensity;
        double radius;

        vec3_t scale;
        vec3_t origin;
        vec3_t angles;
        vec3_t color;

        std::string_view parallaxDepth;
        bool ledsource;


    };

}

template <> struct glz::meta<objects::Light> {

    using T = objects::Light;

    static constexpr auto value = glz::object (

        "visible", &T::visible,
        "name", &T::name,
        "light", &T::light,
        "intensity", &T::intensity,
        "radius", &T::radius,
        "scale", &T::scale,
        "origin", &T::origin,
        "angles", &T::angles,
        "parallaxDepth", &T::parallaxDepth,
        "ledsource", &T::ledsource,
        "color", &T::color

    );

};