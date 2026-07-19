#ifndef ERUPTOR_HARDWARE_VERTRX_HPP
#define ERUPTOR_HARDWARE_VERTRX_HPP

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace eruptor::hardware
{

struct Vertex
{
    glm::vec3 pos{};
    glm::vec3 normals{};
    glm::vec2 texture_cord{};

    static consteval vk::VertexInputBindingDescription Get_binding_descriptor()
    {
        vk::VertexInputBindingDescription description{};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = vk::VertexInputRate::eVertex;

        return description;
    }

    static consteval  std::array<vk::VertexInputAttributeDescription, 3> Get_attribute_descriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 3> attribute_descriptions{};

        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attribute_descriptions[1].offset = offsetof(Vertex, normals);

        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].format = vk::Format::eR32G32Sfloat;
        attribute_descriptions[2].offset = offsetof(Vertex, texture_cord);

        return attribute_descriptions;
    }
};

};

#endif //ERUPTOR_HARDWARE_VERTRX_HPP
