#ifndef ERUPTOR_HARDWARE_CORE_HPP
#define ERUPTOR_HARDWARE_CORE_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <string_view>

namespace eruptor::hardware
{

class Core
{
public:
    void Init(GLFWwindow * window);

    const vk::raii::Instance & Get_instance_handle() {return instance;}
    const vk::raii::SurfaceKHR & Get_surface_handle() {return surface;}

    auto Get_physical_devices() {return instance.enumeratePhysicalDevices();}

private:
    void Create_instance();
    void Setup_debug_messenger();
    void Create_surface(GLFWwindow * window);

    std::vector<const char *> Get_required_instance_extensions();

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                           vk::DebugUtilsMessageTypeFlagsEXT type,
                                                           const vk::DebugUtilsMessengerCallbackDataEXT * p_callback_data,
                                                           void * p_user_data);
    vk::raii::Context context{};
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;

    vk::raii::DebugUtilsMessengerEXT  debug_messenger = nullptr;

    const std::vector<char const*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
};

}

#endif // ERUPTOR_CORE_HPP
