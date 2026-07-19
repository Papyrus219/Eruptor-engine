#ifndef ERUPTOR_HARDWARE_DEVICE_HPP
#define ERUPTOR_HARDWARE_DEVICE_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>

namespace eruptor::hardware
{

class Core;

class Device
{
public:
    void Init(Core & core);

    const vk::raii::Device & Get_device_handle() {return device;};
    const vk::raii::PhysicalDevice & Get_physical_device_handle() {return physical_device;}
    vma::raii::Allocator & Get_alocator_handle() {return alocator;}

    vma::raii::Image Create_image(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
    vk::raii::ImageView Create_image_view(vk::Image const & image, vk::Format format, vk::ImageAspectFlags aspect_flags);

    bool Get_is_one_queue_family();

    vk::SurfaceCapabilitiesKHR Get_surface_capabilities(const vk::raii::SurfaceKHR & surface);
    std::vector<vk::SurfaceFormatKHR> Get_surface_formats(const vk::raii::SurfaceKHR & surface);
    std::vector<vk::PresentModeKHR> Get_surface_present_modes(const vk::raii::SurfaceKHR & surface);

    vk::Format Find_supported_format(const std::vector<vk::Format> & candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

private:
    vk::raii::PhysicalDevice physical_device = nullptr;
    vk::raii::Device device = nullptr;
    vma::raii::Allocator alocator = nullptr;

    void Pick_physical_device(Core & core);
    void Create_logical_device(Core & core);
    void Create_alocator(Core & core);

    bool Is_device_sutiable(const vk::raii::PhysicalDevice & device, Core & core);

    class Queues
    {
    public:
        std::vector<uint32_t> Get_unique_indices();

        uint32_t Get_graphics_queue_index() {return graphics_index;}
        uint32_t Get_transfer_queue_index() {return transfer_index;}
        uint32_t Get_compute_queue_index() {return compute_index;}

        vk::raii::Queue & Get_graphics_queue_handle() {return graphics_queue;}
        vk::raii::Queue & Get_transfer_queue_handle() {return transfer_queue;}
        vk::raii::Queue & Get_compute_queue_handle() {return compute_queue;}

    private:
        uint32_t graphics_index{};
        uint32_t transfer_index{};
        uint32_t compute_index{};

        vk::raii::Queue graphics_queue = nullptr;
        vk::raii::Queue transfer_queue = nullptr;
        vk::raii::Queue compute_queue = nullptr;

        friend class Device;
    };

public:
    Queues queues{};
};

}

#endif // ERUPTOR_HARDWARE_DEVICE_HPP
