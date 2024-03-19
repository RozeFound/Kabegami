#pragma once

#include "glaze/json/json_t.hpp"
#include "vectors.hpp"

namespace objects {

    struct Pass {

        int id;

        std::unordered_map<std::string_view, int> combos;

        std::string_view blending;
        std::string_view cullmode;
        std::string_view depthtest;
        std::string_view depthwrite;
        std::string_view shader;

        std::unordered_map<std::string_view, glz::json_t> constantshadervalues;
        std::vector<std::optional<std::string_view>> textures;

        struct UserTexture {
            std::string_view name;
            std::string_view type;
        };

        std::vector<std::optional<UserTexture>> user_textures;

    };

    struct Effect {

        std::string_view file;
        int id;
        std::string_view name;

        std::vector<Pass> passes;
        std::string_view username;

        struct Visible {
            std::string_view user;
            bool value;
        }; std::variant<bool, Visible> visible;

    };

    struct Model {
        bool autosize;
        bool fullscreen;
        bool passthrough;
        std::string_view material;
        std::string_view puppet;
    };

    struct object_t {

        int id;
        bool visible;
        std::string_view name;

        std::string_view particle;

        vec3_t scale;
        vec3_t origin;
        vec3_t angles;

        bool locktransforms;
        vec2_t parallaxDepth;

        std::vector<std::string_view> sound;
        std::string_view playbackmode;
        double volume;

        double mintime;
        double maxtime;

        bool muteineditor;
        bool startsilent;

        std::string_view light;
        double intensity;
        double radius;

        vec3_t color;

        bool ledsource;

        vec2_t size;

        int colorBlendMode;

        bool copybackground;
        bool perspective;
        bool solid;

        double alpha;
        double brightness;

        std::vector<Effect> effects;
        std::optional<std::string_view> image;
        std::vector<int> dependencies;
        std::string_view alignment;

    struct {
        double alpha;
        double brightness;
        vec3_t color;
        vec3_t colorn;
        vec3_t controlpoint1;
        vec3_t controlpoint2;
        vec3_t controlpoint3;
        vec3_t controlpoint4;
        vec3_t controlpoint7;
        double count;
        std::optional<int> id;
        double lifetime;
        double rate;
        double size;
        double speed;
    } instanceoverride;

    };

}

template <> struct glz::meta<objects::Pass> {

    using T = objects::Pass;

    static constexpr auto value = object(
        "id", &T::id,
        "combos", &T::combos,
        "blending", &T::blending,
        "cullmode", &T::cullmode,
        "depthtest", &T::depthtest,
        "depthwrite", &T::depthwrite,
        "shader", &T::shader,
        "constantshadervalues", &T::constantshadervalues,
        "textures", &T::textures,
        "user_textures", &T::user_textures
    );

};