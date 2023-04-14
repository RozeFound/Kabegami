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

    struct Constants {
        double ui_editor_properties_audio_amount;
        std::string_view ui_editor_properties_audio_bounds;
        float ui_editor_properties_audio_exponent;
        int ui_editor_properties_frequency_max;
        int ui_editor_properties_frequency_min;
        double ui_editor_properties_strength;
    } constantshadervalues;

    std::vector<std::optional<std::string_view>> textures;

};

GLZ_META(Pass, combos, constantshadervalues, textures);
GLZ_META(Pass::Combo, AUDIOPROCESSING, DIRECTION, NOISE);
GLZ_META(Pass::Constants, ui_editor_properties_audio_amount, ui_editor_properties_audio_bounds,
                        ui_editor_properties_audio_exponent, ui_editor_properties_frequency_max,
                        ui_editor_properties_frequency_min, ui_editor_properties_strength);

struct Effect {

    std::string_view file;
    std::vector<Pass> passes;
    std::string_view username;
    bool visible;

};

GLZ_META(Effect, file, passes, username, visible);

struct Object {

    std::string_view angles;
    int colorBlendMode;
    bool copybackground;

    std::vector<Effect> effects;

    int id;
    std::string_view image;
    bool locktransforms;
    std::string_view name;
    std::string_view origin;
    std::string_view parallaxDepth;
    std::string_view scale;
    std::string_view size;
    bool visible;

};

GLZ_META(Object, angles, colorBlendMode, copybackground, effects,
    id, image, locktransforms, name, origin, parallaxDepth, scale, size, visible);

struct Scene {

    struct Camera {
        std::string_view center;
        std::string_view eye;
        std::string_view up;
    } camera;

    struct General {

        std::string_view ambientcolor;
        bool bloom;
        int bloomstrength;
        double bloomthreshold;
        bool camerafade;
        bool cameraparallax;
        float cameraparallaxamount;
        double cameraparallaxdelay;
        int cameraparallaxmouseinfluence;
        bool camerapreview;
        bool camerashake;
        float camerashakeamplitude;
        int camerashakeroughness;
        int camerashakespeed;
        std::string_view clearcolor;
        std::optional<bool> clearenabled;

        struct OrthogonalProjection {
            int height;
            int width;
        } orthogonalprojection;

        std::string_view skylightcolor;

    } general;

    std::vector<Object> objects;

};

GLZ_META(Scene, camera, general, objects);
GLZ_META(Scene::Camera, center, eye, up);
GLZ_META(Scene::General, ambientcolor, bloom, bloomstrength, bloomthreshold,
    camerafade, cameraparallax, cameraparallaxamount, cameraparallaxdelay, cameraparallaxmouseinfluence,
    camerapreview, camerashake, camerashakeamplitude, camerashakeroughness, camerashakespeed,
    clearcolor, clearenabled, orthogonalprojection, skylightcolor);
GLZ_META(Scene::General::OrthogonalProjection, height, width);