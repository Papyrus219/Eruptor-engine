#ifndef ERUPTOR_RENDERER_RENDER_REQUEST_HPP
#define ERUPTOR_RENDERER_RENDER_REQUEST_HPP

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

}

#endif //ERUPTOR_RENDERER_RENDER_REQUEST_HPP
