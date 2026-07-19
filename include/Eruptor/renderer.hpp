#ifndef ERUPTOR_RENDERER_RENDERER_HPP
#define ERUPTOR_RENDERER_RENDERER_HPP

#include <Eruptor/resource_manager.hpp>
#include <Eruptor/resource/resource_handle.hpp>
#include <Eruptor/event/event_listener.hpp>
#include <Eruptor/renderer/cameras.hpp>
#include <vector>

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

    void Begin_frame();
    void End_frame();

    void Render_scene(scene::Scene & scene);

    void On_event(const event::Event & event) override;

    eruptor::hardware::Window & Get_window();
    Fly_camera & Get_camera() {return fly_camera;}

private:
    void Render_model(resource::Model_handle model_handle);

    std::vector<Frame_sync> frame_syncs;

    hardware::Hardware * hardware{};
    resource::Resource_manager * rs_resource_manager{};

    event::Event_manager & event_manager;

    Fly_camera fly_camera{{0.0f, 1.0f, 0.0f}};

    uint32_t current_image_index{};
    uint32_t current_frame{};
};

}

}

#endif // ERUPTOR_RENDERER_RENDERER_HPP
