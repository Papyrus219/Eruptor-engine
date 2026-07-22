#ifndef ERUPTOR_RENDERER_RENDER_QUEUE_HPP
#define ERUPTOR_RENDERER_RENDER_QUEUE_HPP

#include <Eruptor/renderer/render_request.hpp>
#include <Eruptor/hardware/resources/push_constants.hpp>
#include <vector>

namespace eruptor::renderer
{

struct Render_queue
{
    std::vector< Render_request<hardware::Push_constant_opaque> > opaque_queue{};
    std::vector< Render_request<hardware::Push_constant_debug> > debug_queue{};
};

}

#endif //ERUPTOR_RENDERER_RENDER_QUEUE_HPP
