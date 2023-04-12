#pragma once

#include <string>

#include <glaze/glaze.hpp>
#include <glaze/core/macros.hpp>

#include "filesystem/filesystem.hpp"

struct Settings {

    std::string assets;
    std::string package;

    GLZ_LOCAL_META(Settings, assets, package);

};

class Kabegami {

    Settings settings;

    public:

    Kabegami();
    ~Kabegami();

    void run();

};