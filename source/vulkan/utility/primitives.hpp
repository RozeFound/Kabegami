#pragma once

namespace vku {

    struct Vertex {

        glm::vec3 position;
        glm::vec2 texture_coordinates;

        auto operator== (const Vertex& other) const {

            auto pos_eq = position == other.position;
            auto tex_eq = texture_coordinates == other.texture_coordinates;

            return pos_eq && tex_eq;
            
        }

        static auto get_binding_description() {

            auto description = vk::VertexInputBindingDescription {
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = vk::VertexInputRate::eVertex
            };

            return description;

        }

        static auto get_attribute_descriptions() {

            auto descriptions = std::vector {
                vk::VertexInputAttributeDescription {
                    .location = 0,
                    .binding = 0,
                    .format = vk::Format::eR32G32B32Sfloat,
                    .offset = offsetof(Vertex, position)
                },
                vk::VertexInputAttributeDescription {
                    .location = 1,
                    .binding = 0,
                    .format = vk::Format::eR32G32Sfloat,
                    .offset = offsetof(Vertex, texture_coordinates)
                }
            };

            return descriptions;

        }

    };

}