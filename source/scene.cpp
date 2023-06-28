#include "scene.hpp"

Scene::Scene (const Context& info, const assets::FileSystem& fs) {

    camera = { info.camera.center, info.camera.eye, info.camera.up }; 

    for (const auto& object : info.objects) {

        logi("Variant index: {}", object.index());

        if (auto image = std::get_if<objects::Image>(&object))
            logi("Found image: {}", image->image.value());
        else if (auto particle = std::get_if<objects::Particle>(&object))
            logi("Found particle: {}", particle->particle);
        else if (auto sound = std::get_if<objects::Sound>(&object))
            logi("Found sound: {}", sound->sound.at(0));
        else if (auto light = std::get_if<objects::Light>(&object))
            logi("Found light: {}", light->light);

    }
    
}