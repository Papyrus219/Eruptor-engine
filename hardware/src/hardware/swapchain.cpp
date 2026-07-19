#include <Eruptor/hardware/swapchain.hpp>
#include <Eruptor/hardware/core.hpp>
#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/window.hpp>
#include <cstdint>
#include <limits>
#include <algorithm>

void eruptor::hardware::Swapchain::Init(Device & device, Window & window, const vk::raii::SurfaceKHR & surface)
{
    Create_swap_chain(device, window, surface);
    Create_image_views(device);
    Create_depth_resources(device);
}

void eruptor::hardware::Swapchain::Create_swap_chain(Device& device, Window& window, const vk::raii::SurfaceKHR & surface)
{
    vk::SurfaceCapabilitiesKHR surface_capabilities = device.Get_surface_capabilities( surface );
    swap_chain_extent = Choose_swap_extent(window, surface_capabilities);
    uint32_t min_image_count = Choose_swap_min_image_count(surface_capabilities);

    std::vector<vk::SurfaceFormatKHR> avalible_formats = device.Get_surface_formats( surface );
    swap_chain_surface_format = Choose_swap_surface_format(avalible_formats);

    std::vector<vk::PresentModeKHR> avalible_present_modes = device.Get_surface_present_modes( surface );

    auto indices_unique = device.queues.Get_unique_indices();

    vk::SwapchainCreateInfoKHR swap_chain_create_info{};
    swap_chain_create_info.surface = surface;
    swap_chain_create_info.minImageCount = min_image_count;
    swap_chain_create_info.imageFormat = swap_chain_surface_format.format;
    swap_chain_create_info.imageColorSpace = swap_chain_surface_format.colorSpace;
    swap_chain_create_info.imageExtent = swap_chain_extent;
    swap_chain_create_info.imageArrayLayers = 1;
    swap_chain_create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swap_chain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
    swap_chain_create_info.preTransform = surface_capabilities.currentTransform;
    swap_chain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swap_chain_create_info.presentMode = Choose_swap_present_mode( avalible_present_modes );
    swap_chain_create_info.clipped = true;
    swap_chain_create_info.oldSwapchain = nullptr;

    swap_chain = vk::raii::SwapchainKHR{device.Get_device_handle(), swap_chain_create_info};
    swap_chain_images = swap_chain.getImages();
}

void eruptor::hardware::Swapchain::Create_image_views(Device & device)
{
    assert(swap_chain_image_views.empty());

    for(auto & image : swap_chain_images)
    {
        swap_chain_image_views.push_back( device.Create_image_view(image, swap_chain_surface_format.format, vk::ImageAspectFlagBits::eColor) );
    }
}

void eruptor::hardware::Swapchain::Create_depth_resources(Device& device)
{
    vk::Format depth_format = Find_depth_format(device);

    depth_image = device.Create_image(swap_chain_extent.width, swap_chain_extent.height, depth_format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
    depth_image_view = device.Create_image_view(depth_image, depth_format, vk::ImageAspectFlagBits::eDepth);
}

vk::SurfaceFormatKHR eruptor::hardware::Swapchain::Choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR> & avalible_formats)
{
    const auto format_it =
    std::ranges::find_if(avalible_formats, [](const auto & format)
    {
        return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;

    });

    return format_it != avalible_formats.end() ? *format_it : avalible_formats[0];
}

vk::PresentModeKHR eruptor::hardware::Swapchain::Choose_swap_present_mode(const std::vector<vk::PresentModeKHR>& avalible_present_modes)
{
    assert(std::ranges::any_of(avalible_present_modes, [](auto present_mode) {return present_mode == vk::PresentModeKHR::eFifo;}));

    return std::ranges::any_of(avalible_present_modes,
            [](const vk::PresentModeKHR value)
            {
                return vk::PresentModeKHR::eMailbox == value;
            }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D eruptor::hardware::Swapchain::Choose_swap_extent(Window & window, const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    int width{}, height{};
    glfwGetFramebufferSize(window.Get_glfw_window(), &width, &height);
    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

uint32_t eruptor::hardware::Swapchain::Choose_swap_min_image_count(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto min_image_count = std::max(3u, capabilities.minImageCount);
    if((0 < capabilities.maxImageCount) && (capabilities.minImageCount < min_image_count))
    {
        min_image_count = capabilities.maxImageCount;
    }

    return min_image_count;
}

vk::Format eruptor::hardware::Swapchain::Find_depth_format(Device & device)
{
    return device.Find_supported_format(
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
         vk::ImageTiling::eOptimal,
         vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

