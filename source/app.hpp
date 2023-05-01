#pragma once

#include <string>

#include <glaze/glaze.hpp>
#include <glaze/core/macros.hpp>

#include "vulkan/engine.hpp"
#include "window.hpp"

class Kabegami {

    struct Settings {
        std::string assets;
        std::string wallpaper;
        GLZ_LOCAL_META(Settings, assets, wallpaper);
    } settings;

    std::unique_ptr<Window> window;
    std::unique_ptr<Engine> engine;

    public:

    Kabegami();
    ~Kabegami();

    void run();

};