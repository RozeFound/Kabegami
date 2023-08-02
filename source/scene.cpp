#include "scene.hpp"

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
    
}