#ifndef ERUPTOR_PHYSIC_PHYSIC_MANAGER_HPP
#define ERUPTOR_PHYSIC_PHYSIC_MANAGER_HPP

#include <Eruptor/event/event_listener.hpp>

namespace eruptor::scene
{
    struct Scene;
}
namespace eruptor::event
{
    class Event_manager;
}

namespace eruptor::physic
{

class Physic_manager: public event::Event_listener
{
public:
    Physic_manager();
    void Chceck_aabbs_colisions(scene::Scene & scene);

    void On_event(const event::Event & event) override;
private:
    event::Event_manager & event_manager;
};

}

#endif // ERUPTOR_PHYSIC_PHYSIC_MANAGER_HPP
