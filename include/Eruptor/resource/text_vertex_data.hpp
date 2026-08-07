#ifndef ERUPTOR_RESOURCE_TEXT_VERTEX_DATA_HPP
#define ERUPTOR_RESOURCE_TEXT_VERTEX_DATA_HPP

#include <glm/glm.hpp>

namespace eruptor::resource
{

struct Text_vertex_data
{
    glm::vec2 pos{};
    glm::vec2 uv{};
    glm::u8vec4 color{};
};

}

#endif //ERUPTOR_RESOURCE_TEXT_VERTEX_DATA_HPP
