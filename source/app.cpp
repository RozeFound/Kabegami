#include "app.hpp"

#include "filesystem/vfs.hpp"
#include "utils.hpp"
#include "scene.hpp"

Kabegami::Kabegami() {

    window = std::make_unique<Window>("Kabegami", 800, 600);
    engine = std::make_unique<Engine>(*window);

    auto ec = glz::read_file_json(settings, "settings.json", std::string{});

}

Kabegami::~Kabegami() {

    std::string buffer;

    glz::write<glz::opts{.prettify=true}>(settings, buffer);

    auto file = std::ofstream("settings.json");

    file << buffer;

}

void Kabegami::run() {

    auto vfs = fs::VFS { settings.assets, settings.wallpaper };

    if constexpr (debug) vfs.mount_location("./");
    if (!settings.cache_location.empty())
        vfs.mount_location(settings.cache_location, "cache");
    else vfs.mount_location(fs::get_cache_dir(), "cache");

    auto file = glz::get_as_json<std::string, "/file">(vfs.read<std::string>("project.json"));
    if (!vfs.exists(file.value())) vfs.mount_package(settings.wallpaper + "/scene.pkg");

    objects::Scene scene_info;
    auto buffer = vfs.read<std::string>(file.value());
    
    constexpr auto opts = glz::opts{.error_on_unknown_keys=false};
    if (auto pe = glz::read<opts>(scene_info, buffer)) {
        loge("Failed to parse scene:\n{}", glz::format_error(pe, buffer));
        auto file_path = fs::write_temp(buffer);
        logd("Scene json at: {}", file_path.string());
        return;
    }

    engine->set_scene(std::make_shared<Scene>(scene_info, vfs));

    window->add_key_callback([this](int key, int action, int mods) {
        if (mods == GLFW_MOD_CONTROL && action == GLFW_PRESS) {
            if (key == GLFW_KEY_F) window->toggle_fullscreen();
            if (key == GLFW_KEY_Q || key == GLFW_KEY_W) window->close();
        }
    });

    while (window->is_open())
        engine->update();

}