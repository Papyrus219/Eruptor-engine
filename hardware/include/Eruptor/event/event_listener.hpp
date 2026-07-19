#ifndef ERUPTOR_EVENT_EVENT_LISTENER_HPP
#define ERUPTOR_EVENT_EVENT_LISTENER_HPP

#include <Eruptor/event/event.hpp>

namespace eruptor::event
{

class Event_listener
{
public:
    virtual void On_event(const Event & event) = 0;

    virtual ~Event_listener() = default;
};

}

#endif // ERUPTOR_EVENT_EVENT_LISTENER_HPP
