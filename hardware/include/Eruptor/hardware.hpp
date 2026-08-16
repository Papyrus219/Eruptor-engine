#ifndef ERUPTOR_HARDWARE_HARDWARE_HPP
#define ERUPTOR_HARDWARE_HARDWARE_HPP

#include <Eruptor/hardware/core.hpp>
#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/window.hpp>
#include <Eruptor/hardware/swapchain.hpp>
#include <Eruptor/hardware/uniform_buffers.hpp>
#include <Eruptor/hardware/pipelines.hpp>
#include <Eruptor/hardware/command_manager.hpp>
#include <Eruptor/hardware/resources/resource_manager.hpp>
#include <Eruptor/hardware/resources/push_constants.hpp>
#include <Eruptor/hardware/constants.hpp>

namespace eruptor::hardware
{

class Hardware
{
public:
    void Init();
    void Recreate_swapchain();

    Core & Get_core() {return core;}
    Window & Get_window() {return window;}
    Device & Get_device() {return device;}
    Uniform_buffers & Get_uniform_buffers() {return uniform_buffers;}
    Swapchain & Get_swapchain() {return swapchain;}
    Pipelines & Get_pipelines() {return pipelines;}
    Command_manager & Get_command_manager() {return command_manager;}
    Resource_manager & Get_resource_manager() {return resource_manager;}
private:
    Core core{};
    Device device{};
    Window window{};
    Uniform_buffers uniform_buffers{};
    Swapchain swapchain{};
    Pipelines pipelines{};
    Command_manager command_manager{};
    Resource_manager resource_manager{};
};

}

#endif // ERUPTOR_HARDWARE_HARDWARE_HPP
