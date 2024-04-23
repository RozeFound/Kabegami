#pragma once

#include <string>

#include <glaze/glaze.hpp>

#include "vulkan/engine.hpp"
#include "window.hpp"

class Kabegami {

    struct Settings {
        std::string assets;
        std::string wallpaper;
        std::string cache_location;
    } settings;

    std::unique_ptr<Window> window;
    std::unique_ptr<Engine> engine;

    public:

    Kabegami();
    ~Kabegami();

    void run();

};