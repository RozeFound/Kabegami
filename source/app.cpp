#include "app.hpp"

#include <fmt/core.h>

#include "structures.hpp"

Kabegami::Kabegami() {

    auto ec = glz::read_file(settings, "Settings.json");

}

Kabegami::~Kabegami() {

    std::string buffer;

    glz::write<glz::opts{.prettify=true}>(settings, buffer);

    auto file = std::ofstream("Settings.json");

    file << buffer;

}

void Kabegami::run() {

    auto parse_scene = [this](FileSystem& fs) {

        auto file = glz::get_as_json<std::string, "/file">(fs.read_as_string("project.json"));

        if (!fs.exists(*file)) fs.add_package(settings.wallpaper + "/scene.pkg");

        auto scene = glz::read_json<Scene>(fs.read_as_string(*file));
        if (!scene) throw std::runtime_error("Failed to parse scene");

        return scene;

    };

    auto fs = FileSystem();
    fs.add_location(settings.assets);
    fs.add_location(settings.wallpaper);

    auto scene = parse_scene(fs);
    
    auto op = scene->general.orthogonal_projection;
    fmt::print("width: {}, heigth: {}\n", op.width, op.height);
    fmt::print("Clear color: {}\n", scene->general.clear_color);
    fmt::print("camera preview: {}\n", scene->camera.preview);

}