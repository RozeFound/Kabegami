#include "app.hpp"

#include <ranges>
#include <fmt/core.h>

#include "assets/mappings.hpp"
#include "assets/texture.hpp"

#include "vulkan/core/context.hpp"

Kabegami::Kabegami() {

    setup_logger();

    window = std::make_unique<Window>("Kabegami", 800, 600);

    auto ec = glz::read_file(settings, "settings.json");

    auto context = vki::Context(*window);

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

    auto scene = glz::read_json<Scene>(fs.read<std::string>(*file));
    if (!scene) loge("Failed to parse scene");

    logi("Camera eye: {}", scene->camera.eye.to_string());

    static auto has_image = [] (auto& object) { return object.image.has_value(); };
    for (const auto& object : scene->objects | std::views::filter(has_image)) {
        
        auto model = glz::read_json<Model>(fs.read<std::string>(object.image.value()));
        auto material = glz::read_json<Material>(fs.read<std::string>(model->material));

        for (const auto& tex_name : material->passes 
        | std::views::transform(&Pass::textures) | std::views::join 
        | std::views::filter([] (auto& texture) { return texture.has_value(); })) {

            auto parent = model->material.substr(0, model->material.find_last_of('/'));
            auto path = fmt::format("{}/{}.tex", parent, tex_name.value());

            if (!fs.exists(path)) continue;
            
            auto data = fs.read(path);
            auto texture = std::make_unique<Texture>(data);

            logi("texv: {}, texi: {}", texture->get_header().version, texture->get_header().index);
            logi("width: {}, height: {}", texture->get_header().width, texture->get_header().height);

        }
        
    }

    window->add_key_callback([this](int key, int action, int mods) {
        if (mods == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
            if (key == GLFW_KEY_F) window->toggle_fullscreen();
            if (key == GLFW_KEY_Q || key == GLFW_KEY_W) window->close();
        }
    });

    while (window->is_open()) {
        // main loop basically
        window->close();
    }

}