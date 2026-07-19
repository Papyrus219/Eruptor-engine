#ifndef ERUPTOR_HARDWARE_COMMAND_MANAGER_HPP
#define ERUPTOR_HARDWARE_COMMAND_MANAGER_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <span>

namespace eruptor::hardware
{

class Device;

class Command_manager
{
public:
    void Init(Device & device, int frames_amount);

    vk::raii::CommandBuffer & Begin_graphic_command_record(int current_frame);
    vk::raii::CommandBuffer & Begin_compute_command_record(int current_frame);
    vk::raii::CommandBuffer & Begin_transfer_command_record();
    vk::raii::CommandBuffer & Begin_ownership_graphic_command_record();

    vk::raii::CommandBuffer & Get_graphic_command_buffor(int current_frame);
    vk::raii::CommandBuffer & Get_compute_command_buffor(int current_frame);

    void End_command_record(vk::raii::CommandBuffer & command_buffer);

    void Submit_graphic_commands(vk::raii::CommandBuffer & command_buffer, std::span<const vk::PipelineStageFlags> wait_stages = {}, std::span<const vk::Semaphore> wait_semafores = {}, std::span<const vk::Semaphore> signal_semafores = {}, vk::Fence fence = nullptr);
    void Submit_compute_commands(vk::raii::CommandBuffer & command_buffer, std::span<const vk::PipelineStageFlags> wait_stages = {}, std::span<const vk::Semaphore> wait_semafores = {}, std::span<const vk::Semaphore> signal_semafores = {}, vk::Fence fence = nullptr);
    void Submit_transfer_commands(vk::raii::CommandBuffer & command_buffer, std::span<const vk::PipelineStageFlags> wait_stages = {}, std::span<const vk::Semaphore> wait_semafores = {}, std::span<const vk::Semaphore> signal_semafores = {}, vk::Fence fence = nullptr);

private:
    Device * device{};

    vk::raii::CommandPool compute_command_pool = nullptr;
    vk::raii::CommandPool graphic_command_pool = nullptr;
    vk::raii::CommandPool transfer_command_pool = nullptr;

    std::vector<vk::raii::CommandBuffer> graphics_command_buffers{};
    std::vector<vk::raii::CommandBuffer> compute_command_buffers{};
    vk::raii::CommandBuffer graphic_ownership_command_buffor = nullptr;
    vk::raii::CommandBuffer transfer_command_buffer = nullptr;
};

}

#endif // ERUPTOR_HARDWARE_COMMAND_MANAGER_HPP
