#ifndef ERUPTOR_HARDWARE_VERTRX_HPP
#define ERUPTOR_HARDWARE_VERTRX_HPP

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace eruptor::hardware
{

struct Opaque_vertex
{
    glm::vec3 pos{};
    glm::vec3 normals{};
    glm::vec2 texture_cord{};

    static consteval vk::VertexInputBindingDescription Get_binding_descriptor()
    {
        vk::VertexInputBindingDescription description{};
        description.binding = 0;
        description.stride = sizeof(Opaque_vertex);
        description.inputRate = vk::VertexInputRate::eVertex;

        return description;
    }

    static consteval std::array<vk::VertexInputAttributeDescription, 3> Get_attribute_descriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions{};

        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attribute_descriptions[0].offset = offsetof(Opaque_vertex, pos);

        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attribute_descriptions[1].offset = offsetof(Opaque_vertex, normals);

        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].format = vk::Format::eR32G32Sfloat;
        attribute_descriptions[2].offset = offsetof(Opaque_vertex, texture_cord);

        return attribute_descriptions;
    }
};

struct Text_vertex
{
    glm::vec2 pos{};
    glm::vec2 uv{};
    glm::u8vec4 color{};

    static consteval vk::VertexInputBindingDescription Get_binding_descriptor()
    {
        vk::VertexInputBindingDescription description{};
        description.binding = 0;
        description.stride = sizeof(Text_vertex);
        description.inputRate = vk::VertexInputRate::eVertex;

        return description;
    }

    static consteval std::array<vk::VertexInputAttributeDescription, 3> Get_attribute_descriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions{};

        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].format = vk::Format::eR32G32Sfloat;
        attribute_descriptions[0].offset = offsetof(Text_vertex, pos);

        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].format = vk::Format::eR32G32Sfloat;
        attribute_descriptions[1].offset = offsetof(Text_vertex, uv);

        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].format = vk::Format::eR8G8B8A8Unorm;
        attribute_descriptions[2].offset = offsetof(Text_vertex, color);

        return attribute_descriptions;
    }
};

};

#endif //ERUPTOR_HARDWARE_VERTRX_HPP
