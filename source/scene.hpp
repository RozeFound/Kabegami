#pragma once

#include "assets/filesystem.hpp"

#include "objects/scene.hpp"

class Scene {

    struct { 
        glm::vec3 center;
        glm::vec3 eye;
        glm::vec3 up;
    } camera; 

    std::vector <objects::Image> images;
    std::vector <objects::Particle> particles;
    std::vector <objects::Sound> sounds;
    std::vector <objects::Light> lights;

    public:
    
    std::array<float, 4> clear_color;

    Scene (const objects::Scene& context, const assets::FileSystem& fs);

};
