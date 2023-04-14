#pragma once

#include <string>

#include <glaze/glaze.hpp>
#include <glaze/core/macros.hpp>

#include "filesystem/filesystem.hpp"

struct Settings {

    std::string assets;
    std::string wallpaper;

    GLZ_LOCAL_META(Settings, assets, wallpaper);

};

class Kabegami {

    Settings settings;

    public:

    Kabegami();
    ~Kabegami();

    void run();

};