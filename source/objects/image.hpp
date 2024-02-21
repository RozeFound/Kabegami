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
        std::vector<int> dependencies;
        std::string_view alignment;

    };

}

template <> struct glz::meta<objects::Effect> {

    using T = objects::Effect;

    static constexpr auto value = object (

        "id", &T::id,
        "file", &T::file,
        "passes", &T::passes,
        "username", &T::username,
        "name", &T::name,
        "visible", &T::visible

    );

};