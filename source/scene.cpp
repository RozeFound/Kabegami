#include "scene.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "objects/model.hpp"
#include "objects/material.hpp"

#include "vulkan/utility/primitives.hpp"

Scene::Scene (const objects::Scene& info, const assets::FileSystem& fs) {

    auto width = info.general.orthogonal_projection.width;
    auto height = info.general.orthogonal_projection.height;

    projection = glm::ortho<float>(-width / 2.0, width / 2.0, -height / 2.0, height / 2.0, info.general.nearz, info.general.farz);
    projection = glm::translate(projection, glm::vec3(info.camera.eye)); projection[1][1] *= -1;

    clear_color = { (float)info.general.clear_color.r,
                    (float)info.general.clear_color.g,
                    (float)info.general.clear_color.b, 1.f }; 

    auto _waterripple_frag_shader = "shaders/effects/waterripple_opengl.frag";
    auto _waterripple_vert_shader = "shaders/effects/waterripple_opengl.vert";

    shaders = { _waterripple_frag_shader, _waterripple_vert_shader };

    textures = {
        std::make_pair("water", std::make_shared<vku::Texture>("materials/water-intact.png")),
        std::make_pair("mud", std::make_shared<vku::Texture>("materials/plant-on-water.png")),
        std::make_pair("background", std::make_shared<vku::Texture>("materials/top-part.png")),
        std::make_pair("waterripple_normal", std::make_shared<vku::Texture>("materials/effects/waterripplenormal.png")),
        std::make_pair("white", std::make_shared<vku::Texture>("materials/white.png"))
    };

    float xright = 1920.0f / 2.0f;
    float xleft = -xright;
    float ztop = 1080.0f / 2.0f;
    float zbottom = -xright;

    auto vertecies = std::vector<vku::Vertex> {
        {{xleft, ztop, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{xright, ztop, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{xleft, zbottom, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{xright, zbottom, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    };

    auto indecies = std::vector<uint32_t> {
        0, 1, 2,
        1, 3, 2
    };

    vertex_buffer = std::make_unique<vku::DeviceBuffer>(vertecies.size() * sizeof(vku::Vertex), vk::BufferUsageFlagBits::eVertexBuffer);
    vertex_buffer->upload(vertecies.data(), vertecies.size() * sizeof(vku::Vertex));

    index_buffer = std::make_unique<vku::DeviceBuffer>(indecies.size() * sizeof(uint32_t), vk::BufferUsageFlagBits::eIndexBuffer);
    index_buffer->upload(indecies.data(), indecies.size() * sizeof(uint32_t));

    time = std::chrono::high_resolution_clock::now();

}

void Scene::bind (const vk::raii::CommandBuffer& commands, const vk::raii::Pipeline& pipeline, const vk::raii::PipelineLayout& layout) const {

    std::chrono::time_point<std::chrono::high_resolution_clock> endTimepoint = std::chrono::high_resolution_clock::now();

    int64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(time).time_since_epoch().count();
    int64_t end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

    int64_t duration = end - start;
    float ms = duration * 0.001;

    auto vertex_globals = VertexPC {
        .g_ModelViewProjectionMatrix = projection,
        .g_Time = duration / 1000.0f,
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
        **textures.at("water")->get_descriptor_set(),
        **textures.at("waterripple_normal")->get_descriptor_set(),
        **textures.at("white")->get_descriptor_set()
     };    
    
    commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, sets, nullptr);
    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint32);

}

void Scene::draw (const vk::raii::CommandBuffer& commands) const {

    commands.drawIndexed(6, 1, 0, 0, 0);

}
