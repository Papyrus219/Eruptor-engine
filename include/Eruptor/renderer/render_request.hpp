#ifndef ERUPTOR_RENDERER_RENDER_REQUEST_HPP
#define ERUPTOR_RENDERER_RENDER_REQUEST_HPP

#include <Eruptor/hardware/resources/vertex.hpp>

namespace eruptor::renderer
{

template<typename Push_constant_T>
struct Render_request
{
    uint32_t vertex_offset{};
    uint32_t indices_offset{};

    uint32_t indices_amount{};

    uint32_t material_id{};

    Push_constant_T push_constant{};
};

struct Text_render_request
{
    std::vector< hardware::Text_vertex > vertices{};
    uint32_t font_texture_id{};
};

}

#endif //ERUPTOR_RENDERER_RENDER_REQUEST_HPP
