#pragma once

#include <vector>
#include <string_view>
#include <optional>

#include <glaze/glaze.hpp>

template <typename T> struct vec_t {

    constexpr virtual T& operator[] (const std::ptrdiff_t index) = 0;

    constexpr const T& operator[] (const std::ptrdiff_t index) const { return operator[](index); }

    constexpr T& at (const std::ptrdiff_t index) { return operator[](index); }
    constexpr const T& at (const std::ptrdiff_t index) const { return operator[](index); }

    constexpr virtual std::string string() const = 0;
    constexpr virtual std::size_t size() const noexcept = 0;

};

struct vec2_t : public vec_t <double> {

    double x, y;

    constexpr double& operator[] (const std::ptrdiff_t index) override {

		switch (index) {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range("Out of bound vec2_t index");
		}
	}

    constexpr std::string string() const override { return fmt::format("{:.2f}, {:.2f}", x, y); }
    constexpr std::size_t size() const noexcept override { return 2; }

};

struct vec3_t : public vec_t <double> {

    double x, y, z;

    constexpr operator glm::vec3() const noexcept { return { x, y, z }; }

    constexpr double& operator[] (const std::ptrdiff_t index) override {

		switch (index) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Out of bound vec3_t index");
		}
	}

    constexpr std::string string() const override { return fmt::format("{:.2f}, {:.2f}, {:.2f}", x, y, z); }
    constexpr std::size_t size() const noexcept override { return 3; }

};

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

struct SceneInfo {

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

struct Model {
    bool autosize;
    bool fullscreen;
    bool passthrough;
    std::string_view material;
    std::string_view puppet;
};

struct Material {
    std::vector<Pass> passes;
};