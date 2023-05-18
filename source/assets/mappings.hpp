#pragma once

#include <glaze/core/macros.hpp>

#include "structures.hpp"

GLZ_META(Pass, id, combos, blending, cullmode, depthtest, depthwrite, shader, constantshadervalues, textures);
GLZ_META(Pass::Combo, AUDIOPROCESSING, DIRECTION, NOISE, VERTICAL, version);

GLZ_META(Effect, id, file, passes, username, name, visible);
GLZ_META(Effect::Visible, user, value);

GLZ_META(Object, id, visible, name, alignment, scale, size, origin, angles, color, colorBlendMode,
                 copybackground, locktransforms, perspective, ledsource, solid, sound, playbackmode, volume,
                 muteineditor, startsilent, mintime, maxtime, effects, image, model, light, brightness, intensity, 
                 alpha, particle, parallaxDepth, radius, instanceoverride);
GLZ_META(Object::Override, id, count, rate, size, lifetime, speed, alpha, colorn);

GLZ_META(SceneInfo, camera, general, objects, version);
GLZ_META(SceneInfo::Camera, center, eye, up, paths);

template <> struct glz::meta<SceneInfo::General> {

    using T = SceneInfo::General;

    static constexpr auto value = glz::object (

        "bloom", [] (auto& g) -> auto& { return g.bloom.enabled; },
        "bloomstrength", [] (auto& g) -> auto& { return g.bloom.strength; },
        "bloomthreshold", [] (auto& g) -> auto& { return g.bloom.threshold; },

        "bloomhdrfeather", [] (auto& g) -> auto& { return g.bloom.hdr.feather; },
        "bloomhdrscatter", [] (auto& g) -> auto& { return g.bloom.hdr.scatter; },
        "bloomhdrstrength", [] (auto& g) -> auto& { return g.bloom.hdr.strength; },
        "bloomhdrthreshold", [] (auto& g) -> auto& { return g.bloom.hdr.threshold; },

        "norecompile", &T::norecompile,
        "hdr", &T::hdr,

        "nearz", &T::nearz,
        "farz", &T::farz,
        "fov", &T::fov,

        "zoom", &T::zoom,

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

GLZ_META(SceneInfo::General::OrthogonalProjection, height, width);

GLZ_META(Model, autosize, fullscreen, passthrough, material, puppet);

GLZ_META(Material, passes);

constexpr void str_to_vec (vec_t<double>& vec, std::string_view string) {
    for (std::size_t i = 0, last_id = 0; i < vec.size(); i++) {
        auto delimiter_position = string.find(' ', last_id+1);
        auto str = string.substr(last_id, delimiter_position);
        vec.at(i) = std::stod(str.data());
        last_id = delimiter_position;
    }
}

namespace glz::detail {
    template <> struct from_json <vec2_t> {
    template <auto Opts> static void op (vec2_t& vec, auto&&... args) {
        std::string_view string = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; 
        read<json>::op<Opts>(string, args...);
        str_to_vec(vec, string);
    }};
    template <> struct from_json <vec3_t> {
    template <auto Opts> static void op (vec3_t& vec, auto&&... args) {
        std::string_view string = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; 
        read<json>::op<Opts>(string, args...);
        str_to_vec(vec, string);
    }};
}