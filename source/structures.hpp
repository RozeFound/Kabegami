#pragma once

#include <string_view>
#include <optional>
#include <vector>

#include <glaze/glaze.hpp>
#include <glaze/core/macros.hpp>

struct vec2_t {

    double x, y;

    constexpr double& operator[] (const std::ptrdiff_t index) {

		switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Out of bound vec2_t index");
		}
	}

	constexpr const double& operator[] (const std::ptrdiff_t index) const { return this->operator[](index); }

    constexpr double& at (const std::ptrdiff_t index) { return operator[](index); }
    constexpr const double& at (const std::ptrdiff_t index) const { return operator[](index); }

    std::string to_string() { return fmt::format("{:.2f}, {:.2f}", x, y); }

};

struct vec3_t {

    double x, y, z;

    constexpr double& operator[] (const std::ptrdiff_t index) {

		switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Out of bound vec3_t index");
		}
	}

    constexpr const double& operator[] (const std::ptrdiff_t index) const { return this->operator[](index); }

    constexpr double& at (const std::ptrdiff_t index) { return operator[](index); }
    constexpr const double& at (const std::ptrdiff_t index) const { return operator[](index); }

    std::string to_string() { return fmt::format("{:.2f}, {:.2f}, {:.2f}", x, y, z); }

};

template <std::size_t size>
void str_to_vec (auto& vec, std::string_view string) {
    for (std::size_t i = 0, last_id = 0; i < size; ++i) {
        auto delimiter_position = string.find(' ');
        vec.at(i) = std::stod(string.substr(last_id, delimiter_position).data());
        last_id = delimiter_position;
    }
}

namespace glz::detail {
    template <> struct from_json<vec2_t> {
    template <auto Opts> static void op (vec2_t& vec, auto&&... args) {
        std::string_view string = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; 
        read<json>::op<Opts>(string, args...);
        str_to_vec<2>(vec, string);
      }
   };
    template <> struct from_json<vec3_t> {
    template <auto Opts> static void op (vec3_t& vec, auto&&... args) {
        std::string_view string = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; 
        read<json>::op<Opts>(string, args...);
        str_to_vec<3>(vec, string);
      }
   };
}

struct Pass {

    int id;

    struct Combo {
        int AUDIOPROCESSING;
        int DIRECTION;
        int NOISE;
        int VERTICAL;
        int version;
    } combos;

    std::string_view blending;
    std::string_view cullmode;
    std::string_view depthtest;
    std::string_view depthwrite;
    std::string_view shader;

    std::unordered_map<std::string, glz::json_t> constantshadervalues;
    std::vector<std::optional<std::string_view>> textures;

};

GLZ_META(Pass, id, combos, blending, cullmode, depthtest, depthwrite, shader, constantshadervalues, textures);
GLZ_META(Pass::Combo, AUDIOPROCESSING, DIRECTION, NOISE, VERTICAL, version);

struct Effect {

    int id;
    std::string_view file;
    std::vector<Pass> passes;
    std::string_view username;
    std::string_view name;
    struct Visible {
        std::string_view user;
        bool value;
    };
    std::variant<bool, Visible> visible;

};

GLZ_META(Effect, id, file, passes, username, name, visible);
GLZ_META(Effect::Visible, user, value);

struct Object {

    int id;
    bool visible;
    std::string_view name;

    std::string_view alignment;
    vec3_t scale;
    vec2_t size;
    vec3_t origin;
    vec3_t angles;
    vec3_t color;
    int colorBlendMode;

    bool copybackground;
    bool locktransforms;
    bool perspective;
    bool ledsource;
    bool solid;

    std::vector<std::string_view> sound;
    std::string_view playbackmode;
    double volume;
    bool muteineditor;
    bool startsilent;
    double mintime;
    double maxtime;

    std::vector<Effect> effects;

    std::optional<std::string_view> image;
    std::optional<std::string_view> model;

    std::string_view light;
    double brightness;
    double intensity;
    double alpha;

    std::string_view particle;
    std::string_view parallaxDepth;
    double radius;

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

GLZ_META(Object, id, visible, name, alignment, scale, size, origin, angles, color, colorBlendMode,
                 copybackground, locktransforms, perspective, ledsource, solid, sound, playbackmode, volume,
                 muteineditor, startsilent, mintime, maxtime, effects, image, model, light, brightness, intensity, 
                 alpha, particle, parallaxDepth, radius, instanceoverride);
GLZ_META(Object::Override, id, count, rate, size, lifetime, speed, alpha, colorn);

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

    std::vector<Object> objects;

    int version;

};

GLZ_META(Scene, camera, general, objects, version);
GLZ_META(Scene::Camera, center, eye, up, paths);
template <> struct glz::meta<Scene::General> {

    using T = Scene::General;

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
GLZ_META(Scene::General::OrthogonalProjection, height, width);

struct Model {
    bool autosize;
    bool fullscreen;
    bool passthrough;
    std::string_view material;
    std::string_view puppet;
};

GLZ_META(Model, autosize, fullscreen, passthrough, material, puppet);

struct Material {
    std::vector<Pass> passes;
};

GLZ_META(Material, passes);