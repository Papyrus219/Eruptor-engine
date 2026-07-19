#ifndef ERUPTOR_HARDWARE_UTILITIES_HPP
#define ERUPTOR_HARDWARE_UTILITIES_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace eruptor::hardware::utilities
{
    void Transition_image_layout(vk::raii::CommandBuffer & commad_buffer, const vk::Image & image, vk::ImageLayout old_layout, vk::ImageLayout new_layout, uint32_t src_queue, uint32_t dst_queue, vk::ImageAspectFlags image_aspect_flags);
}

#endif //ERUPTOR_HARDWARE_UTILITIES_HPP
