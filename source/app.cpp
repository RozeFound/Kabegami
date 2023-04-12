#include "app.hpp"

#include <fmt/core.h>

Kabegami::Kabegami() {

    auto ec = glz::read_file(settings, "Settings.json");

}

Kabegami::~Kabegami() {

    auto ec = glz::write_file(settings, "Settings.json");

}

void Kabegami::run() {

    auto fs = FileSystem();

    fs.add_location(settings.assets);
    fs.add_package(settings.package);

    fmt::print("\n{}\0", (char*)fs.read_file("shaders/effects/shake.frag").data()); // file in .pkg
    fmt::print("<------------------------------------------------------------------------------>");
    fmt::print("\n{}\0", (char*)fs.read_file("scenes/gifs/gifscene.json").data()); // file on disk

}