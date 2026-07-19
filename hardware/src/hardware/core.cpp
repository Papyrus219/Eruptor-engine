#include <Eruptor/hardware/core.hpp>
#include <Eruptor/hardware/globals.hpp>
#include <iostream>

using namespace eruptor::hardware;

void eruptor::hardware::Core::Init(GLFWwindow * window)
{
    Create_instance();
    Setup_debug_messenger();
    Create_surface(window);
}

void eruptor::hardware::Core::Create_surface(GLFWwindow* window)
{
    VkSurfaceKHR _surface{};
    if(glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
    {
        throw std::runtime_error{"failed to create window surface!"};
    }

    surface = vk::raii::SurfaceKHR{instance, _surface};
}

void eruptor::hardware::Core::Create_instance()
{
    vk::ApplicationInfo app_info{};
    app_info.pApplicationName = "idk";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Eruptor";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = vk::ApiVersion14;

    std::vector<char const *> required_layers{};
    if(enableValidationLayers)
    {
        required_layers.assign(validation_layers.begin(), validation_layers.end());
    }

    auto layers_properties = context.enumerateInstanceLayerProperties();
    auto unsuported_layer_it = std::ranges::find_if(required_layers,
    [&layers_properties](auto const & required_layer)
    {
        return std::ranges::none_of(layers_properties,
        [&required_layer](auto const & layer_property)
        {
            return strcmp(layer_property.layerName, required_layer) == 0;
        });
    });
    if(unsuported_layer_it != required_layers.end())
    {
        throw std::runtime_error{"Required validation layer not supported: " + std::string(*unsuported_layer_it)};
    }

    auto requried_extensions = Get_required_instance_extensions();

    auto extension_properties = context.enumerateInstanceExtensionProperties();
    auto unsuported_property_it = std::ranges::find_if(requried_extensions,
    [&extension_properties](auto const & required_extension)
    {
        return std::ranges::none_of(extension_properties,
                [required_extension](auto const &extension_property)
                {
                    return strcmp(extension_property.extensionName, required_extension);
                });
    });
    if(unsuported_property_it != requried_extensions.end())
    {
        throw std::runtime_error{"Required GLFW extension not supported: " + std::string(*unsuported_property_it)};
    }

    vk::InstanceCreateInfo create_info{};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());
    create_info.ppEnabledLayerNames = required_layers.data();
    create_info.enabledExtensionCount = static_cast<uint32_t>(requried_extensions.size());
    create_info.ppEnabledExtensionNames = requried_extensions.data();

    instance = vk::raii::Instance{context, create_info};
}

void eruptor::hardware::Core::Setup_debug_messenger()
{
    if(!enableValidationLayers) return;

    vk::DebugUtilsMessageSeverityFlagsEXT severity_flags{vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError};
        vk::DebugUtilsMessageTypeFlagsEXT message_type_flags{vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation};
            vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info_ext{};
            debug_utils_messenger_create_info_ext.messageSeverity = severity_flags;
            debug_utils_messenger_create_info_ext.messageType = message_type_flags;
            debug_utils_messenger_create_info_ext.pfnUserCallback = &debug_callback;

            debug_messenger = instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info_ext);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL eruptor::hardware::Core::debug_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                                        vk::DebugUtilsMessageTypeFlagsEXT type,
                                                                        const vk::DebugUtilsMessengerCallbackDataEXT * p_callback_data,
                                                                        void * p_user_data)
{
    std::cerr << "validation layer: type " << vk::to_string(type) << "msg: " << p_callback_data->pMessage << '\n';

    return vk::False;
}

std::vector<const char *> eruptor::hardware::Core::Get_required_instance_extensions()
{
    uint32_t glfw_extension_count{};
    auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
    if(enableValidationLayers)
    {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}


