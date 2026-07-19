#ifndef ERUPTOR_EVENT_EVENT_MANAGER_HPP
#define ERUPTOR_EVENT_EVENT_MANAGER_HPP

#include <Eruptor/event/event.hpp>
#include <Eruptor/event/event_listener.hpp>
#include <vector>

namespace eruptor::event
{

class Event_manager
{
public:
    void Add_listener(Event_listener & listener);
    void Announce_event(const Event & event);

private:
    std::vector<Event_listener *> listeners{};
};

extern Event_manager event_manager;

}

#endif // ERUPTOR_EVENT_EVENT_MANAGER_HPP
