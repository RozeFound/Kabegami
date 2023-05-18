#pragma once

#include "assets/filesystem.hpp"
#include "assets/mappings.hpp"
#include "vulkan/utility/image.hpp"

class Scene {

    struct { 
        glm::vec3 center;
        glm::vec3 eye;
        glm::vec3 up;
    } camera; 

    std::vector <vku::Texture> textures;

    public:

    Scene (const SceneInfo& info, const FileSystem& fs);

};