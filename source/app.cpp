#include "app.hpp"

#include <ranges>
#include <fmt/core.h>

#include "structures.hpp"
#include "texture.hpp"

Kabegami::Kabegami() {

    auto ec = glz::read_file(settings, "settings.json");

}

Kabegami::~Kabegami() {

    std::string buffer;

    glz::write<glz::opts{.prettify=true}>(settings, buffer);

    auto file = std::ofstream("settings.json");

    file << buffer;

}

void Kabegami::run() {

    auto parse_scene = [this](FileSystem& fs) {

        auto file = glz::get_as_json<std::string, "/file">(fs.read<std::string>("project.json"));

        if (!fs.exists(*file)) fs.add_package(settings.wallpaper + "/scene.pkg");

        auto scene = glz::read_json<Scene>(fs.read<std::string>(*file));
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

    for (const auto& object : scene->objects) {
        
        if (!object.image) continue;
        
        auto model = glz::read_json<Model>(fs.read<std::string>(*object.image));
        auto material = glz::read_json<Material>(fs.read<std::string>(model->material));

        static auto has_value = [] (auto& object) { return object.has_value(); };

        for (const auto& pass : material->passes)
            for (const auto& name : pass.textures | std::views::filter(has_value)) {

                auto parent = model->material.substr(0, model->material.find_last_of('/'));
                auto path = fmt::format("{}/{}.tex", parent, *name);

                if (!fs.exists(path)) continue;
                
                auto data = fs.read(path);
                auto texture = std::make_unique<Texture>(data);

                fmt::print("texv: {}, texi: {}\n", texture->get_header().version, texture->get_header().index);
                fmt::print("width: {}, height: {}\n", texture->get_header().width, texture->get_header().height);

            }
        
    }

}