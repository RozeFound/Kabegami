#include "app.hpp"

#include <ranges>
#include <fmt/core.h>

#include "assets/filesystem.hpp"
#include "assets/mappings.hpp"
#include "assets/texture.hpp"

#include "scene.hpp"

Kabegami::Kabegami() {

    window = std::make_unique<Window>("Kabegami", 800, 600);
    engine = std::make_unique<Engine>(*window);

    auto ec = glz::read_file(settings, "settings.json");

}

Kabegami::~Kabegami() {

    std::string buffer;

    glz::write<glz::opts{.prettify=true}>(settings, buffer);

    auto file = std::ofstream("settings.json");

    file << buffer;

}

void Kabegami::run() {

    auto fs = FileSystem { settings.assets, settings.wallpaper };

    auto file = glz::get_as_json<std::string, "/file">(fs.read<std::string>("project.json"));
    if (!fs.exists(file.value())) fs.add_package(settings.wallpaper + "/scene.pkg");

    auto buffer = fs.read<std::string>(file.value());
    auto scene_info = glz::read_json<SceneInfo>(buffer);
    if (!scene_info) loge("Failed to parse scene");

    auto scene = std::make_unique<Scene>(*scene_info, fs);

    window->add_key_callback([this](int key, int action, int mods) {
        if (mods == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
            if (key == GLFW_KEY_F) window->toggle_fullscreen();
            if (key == GLFW_KEY_Q || key == GLFW_KEY_W) window->close();
        }
    });

    while (window->is_open())
        engine->update();

}