#pragma once

#include <string_view>
#include <optional>
#include <vector>

#include <glaze/glaze.hpp>
#include <glaze/core/macros.hpp>

struct Pass {

    struct Combo {
        int AUDIOPROCESSING;
        int DIRECTION;
        int NOISE;
    } combos;

    std::string_view blending;
    std::string_view cullmode;
    std::string_view depthtest;
    std::string_view depthwrite;
    std::string_view shader;

    std::unordered_map<std::string, glz::json_t> constantshadervalues;
    std::vector<std::optional<std::string_view>> textures;

};

GLZ_META(Pass, combos, blending, cullmode, depthtest, depthwrite, shader, constantshadervalues, textures);
GLZ_META(Pass::Combo, AUDIOPROCESSING, DIRECTION, NOISE);

struct Effect {

    std::string_view file;
    std::vector<Pass> passes;
    std::string_view username;
    bool visible;

};

GLZ_META(Effect, file, passes, username, visible);

struct Object {

    int id;
    bool visible;
    std::string_view name;

    std::string_view scale;
    std::string_view size;
    std::string_view origin;
    std::string_view angles;
    std::string_view color;
    int colorBlendMode;

    bool copybackground;
    bool locktransforms;

    std::vector<Effect> effects;

    std::string_view image;
    std::string_view model;

    std::string_view light;
    double intensity;

    std::string_view particle;
    std::string_view parallaxDepth;
    double radius;

};

GLZ_META(Object, id, visible, name, scale, size, origin, angles, color, colorBlendMode, copybackground,
                 locktransforms, effects, image, model, light, intensity, particle, parallaxDepth, radius);

struct Scene {

    struct Camera {

        std::string_view center;
        std::string_view eye;
        std::string_view up;

        std::vector<std::string_view> paths;

        bool fade;
        bool preview;

        struct Parallax {
            bool enabled;
            float amount;
            double delay;
            int mouse_influence;
        } parallax;

        struct Shake {
            bool enabled;
            float amplitude;
            int roughness;
            int speed;
        } shake;

    } camera;

    struct General {

        Camera& camera;

        struct Bloom {
            bool enabled;
            int strength;
            double threshold;
        } bloom;

        bool norecompile;

        std::optional<bool> clear;
        std::string_view clear_color;

        std::string_view ambient_color;
        std::string_view skylight_color;

        struct OrthogonalProjection {
            int height;
            int width;
        } orthogonal_projection;

    } general { camera };

    std::vector<Object> objects;

};

GLZ_META(Scene, camera, general, objects);
GLZ_META(Scene::Camera, center, eye, up, paths);
template <> struct glz::meta<Scene::General> {

    using T = Scene::General;

    static constexpr auto value = glz::object (

        "bloom", [] (auto& g) -> auto& { return g.bloom.enabled; },
        "bloomstrength", [] (auto& g) -> auto& { return g.bloom.strength; },
        "bloomthreshold", [] (auto& g) -> auto& { return g.bloom.threshold; },

        "norecompile", &T::norecompile,

        "camerafade", [] (auto& g) -> auto& { return g.camera.fade; },
        "camerapreview", [] (auto& g) -> auto& { return g.camera.preview; },

        "cameraparallax", [] (auto& g) -> auto& { return g.camera.parallax.enabled; },
        "cameraparallaxamount", [] (auto& g) -> auto& { return g.camera.parallax.amount; },
        "cameraparallaxdelay", [] (auto& g) -> auto& { return g.camera.parallax.delay; },
        "cameraparallaxmouseinfluence", [] (auto& g) -> auto& { return g.camera.parallax.mouse_influence; },

        "camerashake", [] (auto& g) -> auto& { return g.camera.shake.enabled; },
        "camerashakeamplitude", [] (auto& g) -> auto& { return g.camera.shake.amplitude; },
        "camerashakeroughness", [] (auto& g) -> auto& { return g.camera.shake.roughness; },
        "camerashakespeed", [] (auto& g) -> auto& { return g.camera.shake.speed; },

        "clearenabled", &T::clear,
        "clearcolor", &T::clear_color,

        "ambientcolor", &T::ambient_color,
        "skylightcolor", &T::skylight_color,

        "orthogonalprojection", &T::orthogonal_projection

    );

};
GLZ_META(Scene::General::OrthogonalProjection, height, width);

struct Model {
    bool autosize;
    std::string_view material;
};

GLZ_META(Model, autosize, material);

struct Material {
    std::vector<Pass> passes;
};

GLZ_META(Material, passes);