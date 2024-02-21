#pragma once

#include "vectors.hpp"

namespace objects {

    struct Particle {

        int id;
        bool visible;
        std::string_view name;

        std::string_view particle;

        vec3_t scale;
        vec3_t origin;
        vec3_t angles;

        bool locktransforms;
        std::string_view parallaxDepth;

        struct Override {
            std::optional<int> id;
            double count;
            double rate;
            double size;
            double lifetime;
            double speed;
            double alpha;
            vec3_t colorn;
            vec3_t controlpoint1;
        } instanceoverride;

    };

}

template <> struct glz::meta<objects::Particle> {

    using T = objects::Particle;

    static constexpr auto value = object (

        "id", &T::id,
        "visible", &T::visible,
        "name", &T::name,
        "particle", &T::particle,
        "scale", &T::scale,
        "origin", &T::origin,
        "angles", &T::angles,
        "locktransforms", &T::locktransforms,
        "parallaxDepth", &T::parallaxDepth,
        "instanceoverride", &T::instanceoverride,

        "image", skip{},
        "model", skip{}

    );

};