#pragma once

#include <glaze/glaze.hpp>

#include "vectors.hpp"

#include "image.hpp"
#include "particle.hpp"
#include "sound.hpp"
#include "light.hpp"

namespace objects {

    struct Scene {

        struct Camera {

            vec3_t center;
            vec3_t eye;
            vec3_t up;

            std::vector<std::string_view> paths;

            bool fade;
            bool preview;

            struct Parallax {
                bool enabled;
                double amount;
                double delay;
                int mouse_influence;
            } parallax;

            struct Shake {
                bool enabled;
                double amplitude;
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
                struct HDR {
                    double feather;
                    double scatter;
                    double strength;
                    double threshold;
                } hdr;
            } bloom;

            bool norecompile;
            bool hdr;

            double nearz;
            double farz;
            double fov;

            double zoom;
            
            std::optional<bool> clear;

            vec3_t clear_color;
            vec3_t ambient_color;
            vec3_t skylight_color;

            struct OrthogonalProjection {
                int height;
                int width;
            } orthogonal_projection;

        } general { camera };

        std::vector<std::variant<objects::Image, objects::Particle, objects::Sound, objects::Light>> objects;

        int version;

    };

}

template <> struct glz::meta<objects::Scene> {

    using T = objects::Scene;

    static constexpr auto value = glz::object (

        "camera", &T::camera,
        "general", &T::general,
        "objects", &T::objects,
        "version", &T::version

    );

};

template <> struct glz::meta<objects::Scene::General> {

    using T = objects::Scene::General;

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

template <> struct glz::meta<objects::Scene::Camera> {

    using T = objects::Scene::Camera;

    static constexpr auto value = glz::object (

        "center", &T::center,
        "eye", &T::eye,
        "up", &T::up,
        "paths", &T::paths

    );

};

template <> struct glz::meta<objects::Scene::General::OrthogonalProjection> {

    using T = objects::Scene::General::OrthogonalProjection;

    static constexpr auto value = glz::object (

        "height", &T::height,
        "width", &T::width

    );

};
