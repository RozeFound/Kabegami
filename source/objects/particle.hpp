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
            int id;
            double count;
            double rate;
            double size;
            double lifetime;
            double speed;
            double alpha;
            vec3_t colorn;
        } instanceoverride;

    };

}

template <> struct glz::meta<objects::Particle> {

    using T = objects::Particle;

    static constexpr auto value = glz::object (

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

        "image", glz::skip{},
        "model", glz::skip{}

    );

};

template <> struct glz::meta<objects::Particle::Override> {

    using T = objects::Particle::Override;

    static constexpr auto value = glz::object (
        "id", &T::id,
        "count", &T::count,
        "rate", &T::rate,
        "size", &T::size,
        "lifetime", &T::lifetime,
        "speed", &T::speed,
        "alpha", &T::alpha,
        "colorn", &T::colorn
    );
};