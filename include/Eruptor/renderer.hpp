#ifndef ERUPTOR_RENDERER_RENDERER_HPP
#define ERUPTOR_RENDERER_RENDERER_HPP

#include <Eruptor/resource_manager.hpp>
#include <Eruptor/resource/resource_handle.hpp>
#include <Eruptor/event/event_listener.hpp>
#include <Eruptor/renderer/cameras.hpp>
#include <Eruptor/renderer/render_queue.hpp>
#include <vector>
#include <array>
#include <utility>

namespace eruptor
{
namespace hardware
{
class Hardware;
class Window;
}

namespace scene
{
    struct Scene;
    struct Render_object;
}

namespace event
{
    class Event_manager;
}

namespace renderer
{

struct Frame_sync;

class Renderer: public eruptor::event::Event_listener
{
public:
    Renderer();
    ~Renderer();

    void Init(hardware::Hardware & hardware, resource::Resource_manager & resource_manager);

    void Stage_scene_render_data(scene::Scene & scene);
    void Stage_object_render_data(scene::Render_object & object);

    void Flush_render_buffor();

    void On_event(const event::Event & event) override;

    eruptor::hardware::Window & Get_window();
    Fly_camera & Get_camera() {return fly_camera;}

private:
    std::vector<Frame_sync> frame_syncs;

    Render_queue render_queue{};

    hardware::Hardware * hardware{};
    resource::Resource_manager * rs_resource_manager{};
    hardware::Resource_manager * hw_resource_manager{};

    event::Event_manager & event_manager;

    Fly_camera fly_camera{{0.0f, 1.0f, 0.0f}};

    //uint32_t current_image_index{};
    uint32_t current_frame{};
};

}

}

#endif // ERUPTOR_RENDERER_RENDERER_HPP
