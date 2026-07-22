#include <Eruptor/hardware.hpp>
#include <Eruptor/hardware/memory_units.hpp>

void eruptor::hardware::Hardware::Init()
{
    using namespace units;

    window.Init();
    window.Create_window( "Test", {800,600} );
    core.Init(window.Get_glfw_window());
    device.Init(core);
    swapchain.Init(device, window, core.Get_surface_handle());
    uniform_buffers.Init(device, MAX_FRAMES_IN_FLIGHT);
    command_manager.Init(device, MAX_FRAMES_IN_FLIGHT);
    resource_manager.Assign_device( device );
    resource_manager.Assign_command_manager( command_manager );
    resource_manager.Init(device.Get_alocator_handle(), 256_MiB, 256_MiB, 2_GiB );

    pipelines.Init(device, swapchain, uniform_buffers, resource_manager);
}
