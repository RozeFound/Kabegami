#include "scene.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "objects/model.hpp"
#include "objects/material.hpp"

#include "vulkan/utility/primitives.hpp"
#include "vulkan/core/descriptor_set.hpp"

Scene::Scene (const objects::Scene& info, const assets::FileSystem& fs) {

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

    auto vertecies = std::vector<vku::Vertex> {
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-1.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    };

    auto indices = std::vector<uint16_t> { 0, 1, 2, 0, 2, 3 };

    vertex_buffer = std::make_unique<vku::DeviceBuffer>(vertecies.size() * sizeof(vku::Vertex), vk::BufferUsageFlagBits::eVertexBuffer);
    vertex_buffer->upload(vertecies.data(), vertecies.size() * sizeof(vku::Vertex));

    index_buffer = std::make_unique<vku::DeviceBuffer>(indices.size() * sizeof(uint16_t), vk::BufferUsageFlagBits::eIndexBuffer);
    index_buffer->upload(indices.data(), indices.size() * sizeof(uint16_t));

    pipeline_cache = std::make_unique<vku::PipeLineCache>();

    glm::mat4 orthoProjection = glm::ortho( -1.0f, 1.0f,
                                            1.0f, -1.0f,
                                            0.0f, 1.0f);
    glm::mat4 viewMatrix = glm::lookAt( glm::vec3(0.0f, 0.0f, 1.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelViewProjection = orthoProjection * viewMatrix * modelMatrix;

}

void Scene::allocate_resources(const vk::raii::RenderPass& render_pass) {

    auto vertex_pc_range = vk::PushConstantRange {
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset = 0,
        .size = sizeof(VertexPC)
    };

    logv("Vertex PC range: {}-{}", vertex_pc_range.offset, vertex_pc_range.offset + vertex_pc_range.size);

    auto fragment_pc_range = vk::PushConstantRange {
        .stageFlags = vk::ShaderStageFlagBits::eFragment,
        .offset = sizeof(VertexPC),
        .size = sizeof(FragmentPC)
    };

    logv("Fragment PC range: {}-{}", fragment_pc_range.offset, fragment_pc_range.offset + fragment_pc_range.size);

    auto descriptor_set_layout = vku::DescriptorSetLayoutFactory()
        .add_binding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment)
        .create();

    pipeline_layouts["basic"] = vku::PipeLineLayoutFactory()
        .push_constant({
            .stageFlags = vk::ShaderStageFlagBits::eVertex,
            .offset = 0,
            .size = sizeof(glm::mat4)
        })
        .set_layout(**descriptor_set_layout)
        .create();

    pipeline_layouts["water"] = vku::PipeLineLayoutFactory()
        .set_layout(**descriptor_set_layout)
        .set_layout(**descriptor_set_layout)
        .set_layout(**descriptor_set_layout)
        .push_constant(vertex_pc_range)
        .push_constant(fragment_pc_range)
        .create();

    auto shaders_fs = assets::FileSystem();
    shaders_fs.add_location("./shaders/");

    pipelines["basic"] = vku::PipeLineFactory()
        .vertex_binding(vku::Vertex::get_binding_description())
        .vertex_attributes(vku::Vertex::get_attribute_descriptions())
        .color_blend(true,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd)
        .stages({"basic.vert", "basic.frag"}, shaders_fs)
        .create(**pipeline_cache, *pipeline_layouts.at("basic"), render_pass);

    pipelines["water"] = vku::PipeLineFactory()
        .vertex_binding(vku::Vertex::get_binding_description())
        .vertex_attributes(vku::Vertex::get_attribute_descriptions())
        .cull_mode(vk::CullModeFlagBits::eNone)
        .color_blend(true,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd,
            { vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha }, vk::BlendOp::eAdd)
        .stages({"effects/waterripple_opengl.vert", "effects/waterripple_opengl.frag"}, shaders_fs)
        .create(**pipeline_cache, *pipeline_layouts.at("water"), render_pass);

}

void Scene::draw (const vk::raii::CommandBuffer& commands) const {

    auto offsets = std::array<vk::DeviceSize, 1>{};

    static auto start = std::chrono::high_resolution_clock::now();
    auto current = std::chrono::high_resolution_clock::now();

    float delta = std::chrono::duration<float, std::chrono::seconds::period>(current - start).count();

    auto vertex_globals = VertexPC {
        .g_ModelViewProjectionMatrix = modelViewProjection,
        .g_Time = delta,
        .g_Texture0Resolution = {1.0f, 1.0f, 1.0f, 1.0f},
        .g_Texture2Resolution = {1.0f, 1.0f, 1.0f, 1.0f},
        .g_AnimationSpeed = 0.1f,
        .g_Scale = 2.5f,
        .g_ScrollSpeed = 0.0f,
        .g_Direction = 0.0f
    };

    commands.pushConstants<VertexPC>(*pipeline_layouts.at("water"), vk::ShaderStageFlagBits::eVertex, 0, vertex_globals);

    auto fragment_globals = FragmentPC {
        .g_Strength = 0.07f,
        .g_SpecularPower = 1.0f,
        .g_SpecularStrength = 1.0f,
        .g_SpecularColor = {1.0f, 1.0f, 1.0f}
    };

    commands.pushConstants<FragmentPC>(*pipeline_layouts.at("water"), vk::ShaderStageFlagBits::eFragment, sizeof(VertexPC), fragment_globals);

    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("water"));

    auto sets = std::array { 
        *textures.at("water")->get_descriptor_set(),
        *textures.at("waterripple_normal")->get_descriptor_set(),
        *textures.at("white")->get_descriptor_set()
     };    

    commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layouts.at("water"), 0, sets, nullptr);
    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("water"));

    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);
    commands.drawIndexed(6, 1, 0, 0, 0);

    commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layouts.at("basic"), 0, *textures.at("mud")->get_descriptor_set(), nullptr);
    commands.pushConstants<glm::mat4x4>(*pipeline_layouts.at("basic"), vk::ShaderStageFlagBits::eVertex, 0, modelViewProjection);
    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("basic"));

    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);
    commands.drawIndexed(6, 1, 0, 0, 0);

    commands.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layouts.at("basic"), 0, *textures.at("background")->get_descriptor_set(), nullptr);
    commands.pushConstants<glm::mat4x4>(*pipeline_layouts.at("basic"), vk::ShaderStageFlagBits::eVertex, 0, modelViewProjection);
    commands.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipelines.at("basic"));

    commands.bindVertexBuffers(0, ***vertex_buffer, *offsets.data());
    commands.bindIndexBuffer(**index_buffer, 0, vk::IndexType::eUint16);
    commands.drawIndexed(6, 1, 0, 0, 0);

}
