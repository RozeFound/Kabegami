#pragma once

#include "assets/filesystem.hpp"

#include "vulkan/utility/image.hpp"

#include "objects/scene.hpp"

struct VertexPC {
    alignas(16) glm::mat4 g_ModelViewProjectionMatrix;
    alignas(4) float g_Time;
    alignas(16) glm::vec4 g_Texture0Resolution;
    alignas(16) glm::vec4 g_Texture2Resolution;

    alignas(4) float g_AnimationSpeed;
    alignas(4) float g_Scale;
    alignas(4) float g_ScrollSpeed;
    alignas(4) float g_Direction;
};

struct FragmentPC {
    alignas(4) float g_Strength; 
    alignas(4) float g_SpecularPower; 
    alignas(4) float g_SpecularStrength;
    alignas(16) glm::vec3 g_SpecularColor;
};

class Scene {

    std::chrono::time_point<std::chrono::high_resolution_clock> time;

    glm::mat4x4 projection;

    std::unordered_map<std::string_view, std::shared_ptr<vku::Texture>> textures;
    std::vector<std::string_view> shaders;

    std::unique_ptr<vku::DeviceBuffer> vertex_buffer;
    std::unique_ptr<vku::DeviceBuffer> index_buffer;
    
    public:
    
    
    std::array<float, 4> clear_color;

    Scene (const objects::Scene& context, const assets::FileSystem& fs);

    void bind (const vk::raii::CommandBuffer& commands, const vk::raii::Pipeline& pipeline, const vk::raii::PipelineLayout& layout) const;
    void draw (const vk::raii::CommandBuffer& commands) const;

    constexpr const std::vector<std::string_view>& get_shaders() { return shaders; }

};