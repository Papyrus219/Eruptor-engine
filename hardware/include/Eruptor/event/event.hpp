#ifndef ERUPTOR_EVENT_EVENT_HPP
#define ERUPTOR_EVENT_EVENT_HPP

#include <Eruptor/event/keys.hpp>
#include <variant>
#include <concepts>
#include <cstdint>

namespace eruptor::event
{

template <typename T, typename Parent>
concept Is_inner_of = requires {
    typename T::I_am_in;
    requires std::same_as<typename T::I_am_in, Parent>;
};


class Event
{
public:
    Event() = default;

    template<typename T>
    requires Is_inner_of<T, Event>
    Event(const T & subtype);

    template<typename T>
    requires Is_inner_of<T, Event>
    Event(T && subtype);

    template<typename T>
    requires Is_inner_of<T, Event>
    void operator=(const T & sub_type);

    template<typename T>
    requires Is_inner_of<T, Event>
    void operator=(const T && sub_type);


    struct Close_window
    {
        using I_am_in = Event;
    };

    struct Key_pressed
    {
        Key key_type{};

        using I_am_in = Event;
    };

    struct Key_release
    {
        Key key_type{};

        using I_am_in = Event;
    };

    struct Mouse_moved
    {
        float x_offset{};
        float y_offset{};

        using I_am_in = Event;
    };

    struct Mouse_scroll
    {
        float y_offset{};

        using I_am_in = Event;
    };

    struct Collision_occurred
    {
        uint32_t object_a_id;
        uint32_t object_b_id;

        using I_am_in = Event;
    };

    template<typename T>
    requires Is_inner_of<T, Event>
    [[nodiscard]] const bool Is() const;

    template<typename T>
    requires Is_inner_of<T, Event>
    [[nodiscard]] const T * Get_if() const;

    bool processed{};
private:
    std::variant<Close_window,
                 Key_pressed,
                 Key_release,
                 Mouse_moved,
                 Mouse_scroll,
                 Collision_occurred> data{};
};

template<typename T>
requires Is_inner_of<T, Event>
Event::Event(const T & subtype): data{subtype}
{

}

template<typename T>
requires Is_inner_of<T, Event>
Event::Event(T && subtype): data{ std::move(subtype) }
{

}

template<typename T>
requires Is_inner_of<T, Event>
void Event::operator=(const T & sub_type)
{
    data = sub_type;
}

template<typename T>
requires Is_inner_of<T, Event>
void Event::operator=(const T && sub_type)
{
    data = std::move(sub_type);
}

template<typename T>
requires Is_inner_of<T, Event>
const bool Event::Is() const
{
    return std::holds_alternative<T>(data);
}

template<typename T>
requires Is_inner_of<T, Event>
const T * Event::Get_if() const
{
    return std::get_if<T>(&data);
}

};

#endif //ERUPTOR_EVENT_EVENT_HPP
