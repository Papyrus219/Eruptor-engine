#ifndef ERUPTOR_HARDWARE_PLATFORM_WINDOW_HPP
#define ERUPTOR_HARDWARE_PLATFORM_WINDOW_HPP

#include <Eruptor/event/keys.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string_view>

namespace eruptor::event
{
    class Event_manager;
}

namespace eruptor::hardware
{

class Core;

class Window
{
public:
    Window();

    void Init();
    void Create_window(std::string_view title, glm::uvec2 size);

    void Update();

    GLFWwindow * Get_glfw_window() {return window;}
    bool Is_key_pressed(event::Key key);

private:
    static void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void Mouse_callback(GLFWwindow * window, double x_pos, double y_pos);

    event::Event_manager & event_manager;

    GLFWwindow * window{};
};

}

#endif // ERUPTOR_HARDWARE_PLATFORM_WINDOW_HPP
