#include "app.hpp"

#include <fmt/core.h>

#include "structures.hpp"

Kabegami::Kabegami() {

    auto ec = glz::read_file(settings, "Settings.json");

}

Kabegami::~Kabegami() {

    auto ec = glz::write_file(settings, "Settings.json");

}

void Kabegami::run() {

    auto fs = FileSystem();
    auto ec = glz::parse_error();

    fs.add_location(settings.assets);
    fs.add_location(settings.wallpaper);

    glz::json_t project;
    ec = glz::read_json(project, fs.read_as_string("project.json"));

    auto file = project["file"].get<std::string>();

    if (!fs.exists(file)) fs.add_package(settings.wallpaper + "/scene.pkg");

    auto scene = Scene();
    ec = glz::read_json(scene, fs.read_as_string(file));

    auto op = scene.general.orthogonalprojection;
    fmt::print("width: {}, heigth: {}\n", op.width, op.height);

}