#include "scene.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "objects/model.hpp"
#include "objects/material.hpp"

#include "vulkan/utility/primitives.hpp"

Scene::Scene (const objects::Scene& info, const assets::FileSystem& fs) {

    clear_color = { (float)info.general.clear_color.r,
                    (float)info.general.clear_color.g,
                    (float)info.general.clear_color.b, 1.f }; 

    auto _waterripple_frag_shader = "shaders/effects/waterripple_opengl.frag";
    auto _waterripple_vert_shader = "shaders/effects/waterripple_opengl.vert";

    shaders = { _waterripple_vert_shader, _waterripple_frag_shader };

    textures = {
        std::make_pair("water", std::make_shared<vku::Texture>("materials/water-intact.png")),
        std::make_pair("mud", std::make_shared<vku::Texture>("materials/plant-on-water.png")),
        std::make_pair("background", std::make_shared<vku::Texture>("materials/top-part.png")),
        std::make_pair("waterripple_normal", std::make_shared<vku::Texture>("materials/effects/waterripplenormal.png")),
        std::make_pair("white", std::make_shared<vku::Texture>("materials/white.png"))
    };

    auto vertecies = std::vector<vku::Vertex> {
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    };

    auto indices = std::vector<uint16_t> {
        0, 1, 2,
        0, 2, 3
    };

    vertex_buffer = std::make_unique<vku::DeviceBuffer>(vertecies.size() * sizeof(vku::Vertex), vk::BufferUsageFlagBits::eVertexBuffer);
    vertex_buffer->upload(vertecies.data(), vertecies.size() * sizeof(vku::Vertex));

    index_buffer = std::make_unique<vku::DeviceBuffer>(indices.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer);
    index_buffer->upload(indices.data(), indices.size() * sizeof(uint16_t));

}

void Scene::bind (const vk::raii::CommandBuffer& commands, const vk::raii::Pipeline& pipeline, const vk::raii::PipelineLayout& layout) const {

    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();

    float delta = std::chrono::duration<float, std::chrono::seconds::period>(start - current).count();

    glm::mat4 orthoProjection = glm::ortho( -1.0f, 1.0f,
                                            1.0f, -1.0f,
                                            0.0f, 1.0f);
    glm::mat4 viewMatrix = glm::lookAt( glm::vec3(0.0f, 0.0f, 1.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f));

    auto vertex_globals = VertexPC {
        .g_ModelViewProjectionMatrix = orthoProjection * viewMatrix,
        .g_Time = delta,
        .g_Texture0Resolution = {1.0f, 1.0f, 1.0f, 1.0f},
        .g_Texture2Resolution = {1.0f, 1.0f, 1.0f, 1.0f},
        .g_AnimationSpeed = 0.1f,
        .g_Scale = 2.5f,
        .g_ScrollSpeed = 0.0f,
        .g_Direction = 0.0f
    };

    commands.pushConstants<VertexPC>(*layout, vk::ShaderStageFlagBits::eVertex, 0, vertex_globals);

    auto fragment_globals = FragmentPC {
        .g_Strength = 0.07f,
        .g_SpecularPower = 1.0f,
        .g_SpecularStrength = 1.0f,
        .g_SpecularColor = {1.0f, 1.0f, 1.0f}
    };

    commands.pushConstants<FragmentPC>(*layout, vk::ShaderStageFlagBits::eFragment, sizeof(VertexPC), fragment_globals);

    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

    auto offsets = std::array<vk::DeviceSize, 1>{}; 

    auto sets = std::array { 
        *textures.at("water")->get_descriptor_set(),
        *textures.at("waterripple_normal")->get_descriptor_set(),
        *textures.at("white")->get_descriptor_set()
     };    
    
    commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, sets, nullptr);
    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);

}

void Scene::draw (const vk::raii::CommandBuffer& commands) const {

    commands.drawIndexed(6, 1, 0, 0, 0);

}
