#include "scene.hpp"

#include "objects/model.hpp"
#include "objects/material.hpp"

Scene::Scene (const objects::Scene& info, const assets::FileSystem& fs) {

    camera = { info.camera.center, info.camera.eye, info.camera.up }; 

    clear_color = { (float)info.general.clear_color.r,
                    (float)info.general.clear_color.g,
                    (float)info.general.clear_color.b, 1.f }; 

    for (const auto& object : info.objects) {

        if (auto image = std::get_if<objects::Image>(&object))
            images.push_back(*image);
        else if (auto particle = std::get_if<objects::Particle>(&object))
            particles.push_back(*particle);
        else if (auto sound = std::get_if<objects::Sound>(&object))
            sounds.push_back(*sound);
        else if (auto light = std::get_if<objects::Light>(&object))
            lights.push_back(*light);

    }

    for (const auto& image : images) {

        auto model = glz::read_json<objects::Model>(fs.read<std::string>(image.image.value()));
        auto material = glz::read_json<objects::Material>(fs.read<std::string>(model->material));

        for (const auto& texture : material->passes 
        | std::views::transform(&objects::Pass::textures) | std::views::join 
        | std::views::filter([] (auto& texture) { return texture.has_value(); })) {

            auto parent = model->material.substr(0, model->material.find_last_of('/'));
            auto path = fmt::format("{}/{}.tex", parent, texture.value());

            if (!fs.exists(path)) continue;

            auto data = fs.read(path);
            auto parser = assets::TextureParser(data);

            textures.emplace_back(parser);

        }

    }
    
}