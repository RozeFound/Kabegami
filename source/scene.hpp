#pragma once

#include "assets/filesystem.hpp"

#include "vulkan/utility/image.hpp"
#include "vulkan/core/pipeline.hpp"
#include "vulkan/shaders/shader.hpp"

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

    glm::mat4 modelViewProjection;

    std::unordered_map<std::string_view, std::shared_ptr<vku::Texture>> textures;
    std::unordered_map<std::string_view, std::shared_ptr<vku::Shader>> shaders;

    std::unique_ptr<vku::PipeLineCache> pipeline_cache;
    
    std::unordered_map<std::string_view, std::unique_ptr<vk::raii::PipelineLayout>> pipeline_layouts;
    std::unordered_map<std::string_view, std::unique_ptr<vk::raii::Pipeline>> pipelines;

    std::unique_ptr<vku::DeviceBuffer> vertex_buffer;
    std::unique_ptr<vku::DeviceBuffer> index_buffer;
    
    public:
    
    
    std::array<float, 4> clear_color;

    Scene (const objects::Scene& info, const assets::FileSystem& fs);
    void allocate_resources (const vk::raii::RenderPass& render_pass);

    void draw (const vk::raii::CommandBuffer& commands) const;

};