#include "scene.hpp"

Scene::Scene (const SceneInfo& info, const FileSystem& fs) {

    camera = { info.camera.center, info.camera.eye, info.camera.up }; 

    static auto has_image = [] (auto& object) { return object.image.has_value(); };
    for (const auto& object : info.objects | std::views::filter(has_image)) {
        
        auto model = glz::read_json<Model>(fs.read<std::string>(object.image.value()));
        auto material = glz::read_json<Material>(fs.read<std::string>(model->material));

        for (const auto& texture : material->passes 
        | std::views::transform(&Pass::textures) | std::views::join 
        | std::views::filter([] (auto& texture) { return texture.has_value(); })) {

            auto parent = model->material.substr(0, model->material.find_last_of('/'));
            auto path = fmt::format("{}/{}.tex", parent, texture.value());

            if (!fs.exists(path)) continue;
            
            auto data = fs.read(path);
            auto texture_info = TextureInfo(data);

            textures.emplace_back(texture_info);

        }
        
    }

}