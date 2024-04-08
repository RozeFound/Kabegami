#include "scene.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "vulkan/utility/primitives.hpp"

Scene::Scene (const objects::Scene& info, const fs::VFS& vfs) {

    clear_color = { (float)info.general.clear_color.r,
                    (float)info.general.clear_color.g,
                    (float)info.general.clear_color.b, 1.f }; 

    textures = {
        std::make_pair("water", std::make_shared<vku::Texture>("materials/water-intact.png")),
        std::make_pair("mud", std::make_shared<vku::Texture>("materials/plant-on-water.png")),
        std::make_pair("background", std::make_shared<vku::Texture>("materials/top-part.png")),
        std::make_pair("waterripple_normal", std::make_shared<vku::Texture>("materials/effects/waterripplenormal.png")),
        std::make_pair("white", std::make_shared<vku::Texture>("materials/white.png"))
    };

    shaders = {
        std::make_pair("basic", std::make_shared<vku::Shader>(vfs, "shaders/basic")),
        std::make_pair("waterriple", std::make_shared<vku::Shader>(vfs, "shaders/effects/waterripple")),
    };

    auto vertecies = std::vector<vku::Vertex> {
        {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    };

    auto indices = std::vector<uint16_t> { 0, 1, 2, 0, 2, 3 };

    vertex_buffer = std::make_unique<vku::DeviceBuffer>(vertecies.size() * sizeof(vku::Vertex), vk::BufferUsageFlagBits::eVertexBuffer);
    vertex_buffer->upload(vertecies.data(), vertecies.size() * sizeof(vku::Vertex));

    index_buffer = std::make_unique<vku::DeviceBuffer>(indices.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer);
    index_buffer->upload(indices.data(), indices.size() * sizeof(uint16_t));

    pipeline_cache = std::make_unique<vku::PipeLineCache>();

    glm::mat4 orthoProjection = glm::ortho(-1.0f,  1.0f,
                                            1.0f, -1.0f,
                                            0.0f,  1.0f);
    glm::mat4 viewMatrix = glm::lookAt( glm::vec3(0.0f, 0.0f, 1.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelViewProjection = orthoProjection * viewMatrix * modelMatrix;

}

void Scene::allocate_resources (const vk::raii::RenderPass& render_pass) {

    pipelines["basic"] = vku::PipeLineFactory()
        .vertex_binding(vku::Vertex::get_binding_description())
        .vertex_attributes(vku::Vertex::get_attribute_descriptions())
        .color_blend(true,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd)
        .create(**pipeline_cache, *shaders.at("basic"), render_pass);

    pipelines["water"] = vku::PipeLineFactory()
        .vertex_binding(vku::Vertex::get_binding_description())
        .vertex_attributes(vku::Vertex::get_attribute_descriptions())
        .cull_mode(vk::CullModeFlagBits::eNone)
        .color_blend(true,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd)
        .create(**pipeline_cache, *shaders.at("waterriple"), render_pass);

}

void Scene::draw (const vk::raii::CommandBuffer& commands) const {

    auto offsets = std::array<vk::DeviceSize, 1>{};

    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();

    float delta = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("water"));

    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);
    commands.drawIndexed(6, 1, 0, 0, 0);

    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("basic"));

    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);
    commands.drawIndexed(6, 1, 0, 0, 0);

    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("basic"));

    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);
    commands.drawIndexed(6, 1, 0, 0, 0);

}
