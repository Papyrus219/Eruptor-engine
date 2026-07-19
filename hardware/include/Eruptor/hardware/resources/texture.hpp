#ifndef ERUPTOR_HARDWARE_TEXTURE_HPP
#define ERUPTOR_HARDWARE_TEXTURE_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>

namespace eruptor::hardware
{

class Device;

class Texture
{
public:
    void Init(Device & device, int width, int height, vk::Format format, vk::DeviceSize offset_in_stage_buffer);
    void Create_descriptor_set(Device & device, vk::raii::DescriptorPool & pool, const vk::raii::DescriptorSetLayout & layout, const vk::raii::Sampler & sampler);

    vma::raii::Image texture_image = nullptr;
    vk::raii::ImageView texture_view = nullptr;
    vk::raii::DescriptorSet descriptor_set = nullptr;
    vk::Format format;

    vk::DeviceSize offset_in_stage_buffer{};
    vk::DeviceSize image_size{};

    uint32_t width{};
    uint32_t height{};
};

}

#endif // ERUPTOR_HARDWARE_TEXTURE_HPP
