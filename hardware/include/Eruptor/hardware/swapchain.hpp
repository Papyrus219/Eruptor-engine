#ifndef ERUPTOR_HARDWARE_SWAPCHAIN_HPP
#define ERUPTOR_HARDWARE_SWAPCHAIN_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>

namespace eruptor::hardware
{

class Core;
class Device;
class Window;

class Swapchain
{
public:
    void Init(Device & device, Window & window, const vk::raii::SurfaceKHR & surface);

    const vk::raii::SwapchainKHR & Get_swapchain_handle() const {return swap_chain;}
    const vk::SurfaceFormatKHR & Get_surface_format() const {return swap_chain_surface_format;}
    const vk::Extent2D & Get_extent() const {return swap_chain_extent;}

    vk::raii::Image & Get_depth_image() {return depth_image;}
    const vk::raii::ImageView & Get_depth_image_view() {return depth_image_view;}

    uint32_t Get_image_count() {return swap_chain_images.size();}

    vk::Image & Get_image(uint32_t id) {return swap_chain_images[id]; }
    vk::raii::ImageView & Get_image_view(uint32_t id) {return swap_chain_image_views[id];}

    vk::Format Find_depth_format(Device & device);

private:
    void Create_swap_chain(Device & device, Window & window, const vk::raii::SurfaceKHR & surface);
    void Create_image_views(Device & device);
    void Create_depth_resources(Device & device);

    vk::SurfaceFormatKHR Choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR> & avalible_formats);
    vk::PresentModeKHR Choose_swap_present_mode(const std::vector<vk::PresentModeKHR> & avalible_present_modes);
    vk::Extent2D Choose_swap_extent(Window & window, const vk::SurfaceCapabilitiesKHR & capabilities);
    uint32_t Choose_swap_min_image_count(const vk::SurfaceCapabilitiesKHR & capabilities);

    vk::raii::SwapchainKHR swap_chain = nullptr;
    std::vector<vk::Image> swap_chain_images{};
    std::vector<vk::raii::ImageView> swap_chain_image_views{};
    vk::SurfaceFormatKHR swap_chain_surface_format{};
    vk::Extent2D swap_chain_extent{};

    vma::raii::Image depth_image = nullptr;
    vk::raii::ImageView depth_image_view = nullptr;
};

}

#endif // ERUPTOR_HARDWARE_SWAPCHAIN_HPP
