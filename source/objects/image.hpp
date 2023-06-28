#pragma once

#include "vectors.hpp"

namespace objects {

    struct Effect {

        int id;
        std::string_view file;
        std::vector<glz::json_t> passes;
        std::string_view username;
        std::string_view name;
        struct Visible {
            std::string_view user;
            bool value;
        };
        std::variant<bool, Visible> visible;

    };

    struct Image {

        int id;
        bool visible;
        std::string_view name;

        vec2_t size;
        vec3_t scale;
        vec3_t origin;
        vec3_t angles;

        std::string_view parallaxDepth;

        vec3_t color;
        int colorBlendMode;

        bool ledsource;
        bool copybackground;
        bool locktransforms;
        bool perspective;
        bool solid;

        double alpha;
        double brightness;

        std::vector<Effect> effects;
        std::optional<std::string_view> image;
        std::string_view alignment;

    };

}

template <> struct glz::meta<objects::Image> {

    using T = objects::Image;

    static constexpr auto value = glz::object (

        "id", &T::id,
        "visible", &T::visible,
        "name", &T::name,
        "size", &T::size,
        "scale", &T::scale,
        "origin", &T::origin,
        "angles", &T::angles,
        "parallaxDepth", &T::parallaxDepth,
        "color", &T::color,
        "colorBlendMode", &T::colorBlendMode,

        "ledsource", &T::ledsource,
        "copybackground", &T::copybackground,
        "locktransforms", &T::locktransforms,
        "perspective", &T::perspective,
        "solid", &T::solid,
        
        "alpha", &T::alpha,
        "brightness", &T::brightness,
        "effects", &T::effects,
        "image", &T::image,
        "alignment", &T::alignment

    );

};

template <> struct glz::meta<objects::Effect> {

    using T = objects::Effect;

    static constexpr auto value = glz::object (

        "id", &T::id,
        "file", &T::file,
        "passes", &T::passes,
        "username", &T::username,
        "name", &T::name,
        "visible", &T::visible

    );

};

template <> struct glz::meta<objects::Effect::Visible> {

    using T = objects::Effect::Visible;

    static constexpr auto value = glz::object (

        "user", &T::user,
        "value", &T::value
        
    );

};

