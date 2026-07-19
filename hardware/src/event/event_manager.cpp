#include <Eruptor/event/event_manager.hpp>
#include <Eruptor/event/keys.hpp>

eruptor::event::Event_manager eruptor::event::event_manager{};

void eruptor::event::Event_manager::Add_listener(Event_listener & listener)
{
    for(auto ex_listener : listeners)
    {
        if(ex_listener == &listener)
        {
            return;
        }
    }

    listeners.push_back( &listener );
}

void eruptor::event::Event_manager::Announce_event(const Event & event)
{
    for(auto listener : listeners)
    {
        listener->On_event(event);

        if(event.processed) break;
    }
}
