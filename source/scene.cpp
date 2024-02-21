#include "scene.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "objects/model.hpp"
#include "objects/material.hpp"

Scene::Scene (const objects::Scene& info, const assets::FileSystem& fs) {

    auto width = info.general.orthogonal_projection.width;
    auto height = info.general.orthogonal_projection.height;

    auto projection = glm::ortho<float>(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0, info.general.nearz, info.general.farz);
    projection = glm::translate(projection, (glm::vec3)info.camera.eye);

    clear_color = { (float)info.general.clear_color.r,
                    (float)info.general.clear_color.g,
                    (float)info.general.clear_color.b, 1.f }; 

    std::filesystem::path _water_example = "materials/water-intact.png";
    std::filesystem::path _mud_example = "materials/plant-on-water.png";
    std::filesystem::path _background_example = "materials/top-part.png";
    std::filesystem::path _waterripple_normal = "materials/effects/waterripplenormal.png";
    std::filesystem::path _white = "materials/white.png";

    auto _waterripple_frag_shader = "shaders/effects/waterripple_opengl.frag";
    auto _waterripple_vert_shader = "shaders/effects/waterripple_opengl.vert";

    shaders = { _waterripple_frag_shader, _waterripple_vert_shader };

    textures.emplace_back(_water_example);
    textures.emplace_back(_mud_example);
    textures.emplace_back(_background_example);
    textures.emplace_back(_waterripple_normal);
    textures.emplace_back(_white);

    /*

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

    */
    
}
