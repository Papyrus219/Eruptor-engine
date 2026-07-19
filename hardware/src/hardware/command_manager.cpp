#include <Eruptor/hardware/command_manager.hpp>
#include <Eruptor/hardware/device.hpp>

void eruptor::hardware::Command_manager::Init(Device & device, int frames_amount)
{
    this->device = &device;

    vk::CommandPoolCreateInfo graphics_pool_info{};
    vk::CommandPoolCreateInfo transfer_pool_info{};
    vk::CommandPoolCreateInfo compute_pool_info{};

    graphics_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    graphics_pool_info.queueFamilyIndex = device.queues.Get_graphics_queue_index();

    transfer_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    transfer_pool_info.queueFamilyIndex = device.queues.Get_transfer_queue_index();

    compute_pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    compute_pool_info.queueFamilyIndex = device.queues.Get_compute_queue_index();

    graphic_command_pool = vk::raii::CommandPool{device.Get_device_handle(), graphics_pool_info};
    transfer_command_pool = vk::raii::CommandPool{device.Get_device_handle(), transfer_pool_info};
    compute_command_pool = vk::raii::CommandPool{device.Get_device_handle(), compute_pool_info};

    vk::CommandBufferAllocateInfo graphic_command_buffer_allocate_info{};
    vk::CommandBufferAllocateInfo compute_command_buffer_allocate_info{};

    graphic_command_buffer_allocate_info.commandPool = graphic_command_pool;
    graphic_command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
    graphic_command_buffer_allocate_info.commandBufferCount = frames_amount;

    compute_command_buffer_allocate_info.commandPool = compute_command_pool;
    compute_command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
    compute_command_buffer_allocate_info.commandBufferCount = frames_amount;

    graphics_command_buffers = vk::raii::CommandBuffers{device.Get_device_handle(), graphic_command_buffer_allocate_info};
    compute_command_buffers = vk::raii::CommandBuffers{device.Get_device_handle(), compute_command_buffer_allocate_info};

    vk::CommandBufferAllocateInfo transfer_command_buffer_allocate_info{};
    transfer_command_buffer_allocate_info.commandPool = transfer_command_pool;
    transfer_command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
    transfer_command_buffer_allocate_info.commandBufferCount = 1;

    transfer_command_buffer = std::move( vk::raii::CommandBuffers(device.Get_device_handle(), transfer_command_buffer_allocate_info).front() );
    graphic_ownership_command_buffor = std::move( vk::raii::CommandBuffers(device.Get_device_handle(), graphic_command_buffer_allocate_info).front() );
}

vk::raii::CommandBuffer & eruptor::hardware::Command_manager::Begin_graphic_command_record(int current_frame)
{
    graphics_command_buffers[current_frame].reset();
    graphics_command_buffers[current_frame].begin({});

    return graphics_command_buffers[current_frame];
}

vk::raii::CommandBuffer & eruptor::hardware::Command_manager::Begin_compute_command_record(int current_frame)
{
    compute_command_buffers[current_frame].reset();
    compute_command_buffers[current_frame].begin({});

    return compute_command_buffers[current_frame];
}

vk::raii::CommandBuffer & eruptor::hardware::Command_manager::Begin_transfer_command_record()
{
    vk::CommandBufferBeginInfo begin_info{};

    transfer_command_buffer.reset();
    transfer_command_buffer.begin(begin_info);

    return transfer_command_buffer;
}

vk::raii::CommandBuffer & eruptor::hardware::Command_manager::Begin_ownership_graphic_command_record()
{
    vk::CommandBufferBeginInfo begin_info{};

    graphic_ownership_command_buffor.reset();
    graphic_ownership_command_buffor.begin(begin_info);

    return graphic_ownership_command_buffor;
}

vk::raii::CommandBuffer & eruptor::hardware::Command_manager::Get_graphic_command_buffor(int current_frame)
{
    return graphics_command_buffers[ current_frame ];
}

vk::raii::CommandBuffer & eruptor::hardware::Command_manager::Get_compute_command_buffor(int current_frame)
{
    return compute_command_buffers[ current_frame ];
}

void eruptor::hardware::Command_manager::End_command_record(vk::raii::CommandBuffer& command_buffer)
{
    command_buffer.end();
}

void eruptor::hardware::Command_manager::Submit_graphic_commands(vk::raii::CommandBuffer& command_buffer, std::span<const vk::PipelineStageFlags> wait_stages, std::span<const vk::Semaphore> wait_semafores, std::span<const vk::Semaphore> signal_semafores, vk::Fence fence)
{
    vk::SubmitInfo submit_info{};
    submit_info.setCommandBuffers(*command_buffer);

    submit_info.setPWaitDstStageMask( wait_stages.data() );

    submit_info.setPWaitSemaphores( wait_semafores.data() );
    submit_info.setWaitSemaphoreCount( wait_semafores.size() );

    submit_info.setPSignalSemaphores( signal_semafores.data() );
    submit_info.setSignalSemaphoreCount( signal_semafores.size() );

    device->queues.Get_graphics_queue_handle().submit( submit_info, fence );
}

void eruptor::hardware::Command_manager::Submit_compute_commands(vk::raii::CommandBuffer& command_buffer, std::span<const vk::PipelineStageFlags> wait_stages, std::span<const vk::Semaphore> wait_semafores, std::span<const vk::Semaphore> signal_semafores, vk::Fence fence)
{
    vk::SubmitInfo submit_info{};
    submit_info.setCommandBuffers(*command_buffer);

    submit_info.setPWaitDstStageMask( wait_stages.data() );

    submit_info.setPWaitSemaphores( wait_semafores.data() );
    submit_info.setWaitSemaphoreCount( wait_semafores.size() );

    submit_info.setPSignalSemaphores( signal_semafores.data() );
    submit_info.setSignalSemaphoreCount( signal_semafores.size() );

    device->queues.Get_compute_queue_handle().submit( submit_info, fence );
}

void eruptor::hardware::Command_manager::Submit_transfer_commands(vk::raii::CommandBuffer & command_buffer, std::span<const vk::PipelineStageFlags> wait_stages, std::span<const vk::Semaphore> wait_semafores, std::span<const vk::Semaphore> signal_semafores, vk::Fence fence)
{
    vk::SubmitInfo submit_info{};
    submit_info.setCommandBuffers(*command_buffer);

    submit_info.setPWaitDstStageMask( wait_stages.data() );

    submit_info.setPWaitSemaphores( wait_semafores.data() );
    submit_info.setWaitSemaphoreCount( wait_semafores.size() );

    submit_info.setPSignalSemaphores( signal_semafores.data() );
    submit_info.setSignalSemaphoreCount( signal_semafores.size() );

    device->queues.Get_transfer_queue_handle().submit( submit_info, fence );
}
