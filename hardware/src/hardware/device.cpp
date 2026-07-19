#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/core.hpp>
#include <Eruptor/hardware/window.hpp>
#include <Eruptor/hardware/utility_structures.hpp>
#include <map>
#include <set>

void eruptor::hardware::Device::Init(Core & core)
{
    Pick_physical_device(core);
    Create_logical_device(core);
    Create_alocator(core);
}

vma::raii::Image eruptor::hardware::Device::Create_image(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
{
    vk::ImageCreateInfo image_info{};
    image_info.imageType = vk::ImageType::e2D;
    image_info.format = format;
    image_info.extent = vk::Extent3D{width, height, 1};
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = vk::SampleCountFlagBits::e1;
    image_info.tiling = tiling;
    image_info.usage = usage;
    image_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo alloc_info{};
    alloc_info.usage = vma::MemoryUsage::eAuto;

    return alocator.createImage(image_info, alloc_info);
}

vk::raii::ImageView eruptor::hardware::Device::Create_image_view(const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect_flags)
{
    vk::ImageSubresourceRange sub_resource_range{};
    sub_resource_range.aspectMask = aspect_flags;
    sub_resource_range.baseMipLevel = 0;
    sub_resource_range.levelCount = 1;
    sub_resource_range.baseArrayLayer = 0;
    sub_resource_range.layerCount = 1;

    vk::ImageViewCreateInfo view_info{};
    view_info.image = image;
    view_info.viewType = vk::ImageViewType::e2D;
    view_info.format = format;
    view_info.subresourceRange = sub_resource_range;

    return vk::raii::ImageView{device, view_info};
}

bool eruptor::hardware::Device::Get_is_one_queue_family()
{
    return (queues.graphics_index == queues.transfer_index && queues.transfer_index == queues.compute_index);
}


vk::SurfaceCapabilitiesKHR eruptor::hardware::Device::Get_surface_capabilities(const vk::raii::SurfaceKHR & surface)
{
    return physical_device.getSurfaceCapabilitiesKHR( surface );
}

std::vector<vk::SurfaceFormatKHR> eruptor::hardware::Device::Get_surface_formats(const vk::raii::SurfaceKHR & surface)
{
    return physical_device.getSurfaceFormatsKHR( surface );
}

std::vector<vk::PresentModeKHR> eruptor::hardware::Device::Get_surface_present_modes(const vk::raii::SurfaceKHR & surface)
{
    return physical_device.getSurfacePresentModesKHR( surface );
}

vk::Format eruptor::hardware::Device::Find_supported_format(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (const auto format : candidates) {
        vk::FormatProperties props = physical_device.getFormatProperties(format);

        if (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
            ((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)))
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

void eruptor::hardware::Device::Pick_physical_device(Core & core)
{
    auto physical_devices = core.Get_physical_devices();
    if(physical_devices.empty())
    {
        throw std::runtime_error{"failed to find GPUs with Vulkan support!"};
    }

    std::multimap<int, vk::raii::PhysicalDevice> candidates{};
    for(auto & phy_dev : physical_devices)
    {
        auto device_properties = phy_dev.getProperties();
        auto device_features = phy_dev.getFeatures();
        uint32_t score{};

        if(!Is_device_sutiable(phy_dev, core))
        {
            continue;
        }

        if(device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            score += 1000;
        }

        score += device_properties.limits.maxImageDimension2D;

        auto queue_families = phy_dev.getQueueFamilyProperties();

        uint32_t best_transfer_score{};
        uint32_t best_compute_score{};

        for(auto queue_family : queue_families)
        {
            if((queue_family.queueFlags & vk::QueueFlagBits::eTransfer) )
            {
                if(!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics) && !(queue_family.queueFlags & vk::QueueFlagBits::eCompute))
                {
                    best_transfer_score = std::max(best_transfer_score, 500u);
                }
                else if(!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics))
                {
                    best_transfer_score = std::max(best_transfer_score, 250u);
                }
            }

            if((queue_family.queueFlags & vk::QueueFlagBits::eCompute))
            {
                if(!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics) && !(queue_family.queueFlags & vk::QueueFlagBits::eTransfer))
                {
                    best_compute_score = std::max(best_compute_score, 500u);
                }
                else if(!(queue_family.queueFlags & vk::QueueFlagBits::eGraphics))
                {
                    best_compute_score = std::max(best_compute_score, 250u);
                }
            }
        }
        score += best_transfer_score + best_compute_score;

        candidates.insert(std::make_pair(score, phy_dev));
    }

    if(!candidates.empty() && candidates.rbegin()->first > 0)
    {
        physical_device = candidates.rbegin()->second;
    }
    else
    {
        throw std::runtime_error{"failed to find a suitable GPU!"};
    }
}

void eruptor::hardware::Device::Create_logical_device(Core & core)
{
    std::vector<vk::QueueFamilyProperties> queue_family_properties = physical_device.getQueueFamilyProperties();

    Queue_fam_choise transfer_choise{};
    Queue_fam_choise compute_choise{};

    for(uint32_t i{}; i < queue_family_properties.size(); i++)
    {
        if((queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) && (physical_device.getSurfaceSupportKHR(i, core.Get_surface_handle())))
        {
            queues.graphics_index = i;
        }

        if((queue_family_properties[i].queueFlags & vk::QueueFlagBits::eTransfer))
        {
            uint32_t score{};
            if(!(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) && !(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eCompute))
            {
                score = 100;
            }
            else if(!(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics))
            {
                score = 50;
            }
            else
            {
                score = 25;
            }

            if(transfer_choise.score <= score)
            {
                transfer_choise.index = i;
                transfer_choise.score = score;
            }

            if((queue_family_properties[i].queueFlags & vk::QueueFlagBits::eCompute))
            {
                uint32_t score{};
                if(!(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) && !(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eTransfer))
                {
                    score = 100;
                }
                else if(!(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics))
                {
                    score = 50;
                }
                else
                {
                    score = 25;
                }

                if(compute_choise.score <= score)
                {
                    compute_choise.index = i;
                    compute_choise.score = score;
                }
            }
        }
    }

    if(queues.graphics_index == ~0)
    {
        throw std::runtime_error{"Could not find a queue for graphics and present -> terminating"};
    }

    queues.transfer_index = transfer_choise.index;
    queues.compute_index = compute_choise.index;

    std::set<uint32_t> unique_queues{};
    unique_queues.insert(queues.graphics_index);
    unique_queues.insert(queues.transfer_index);
    unique_queues.insert(queues.compute_index);

    std::vector<vk::DeviceQueueCreateInfo> device_queue_create_infos{};

    float priority{1.0f};
    for(auto queue_index : unique_queues)
    {
        vk::DeviceQueueCreateInfo tmp_info{};
        tmp_info.queueFamilyIndex = queue_index;
        tmp_info.queueCount = 1;
        tmp_info.pQueuePriorities = &priority;

        device_queue_create_infos.push_back(tmp_info);
    }

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
    vk::PhysicalDeviceVulkan11Features,
    vk::PhysicalDeviceVulkan13Features,
    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
    feature_chain;
    feature_chain.get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy = true;
    feature_chain.get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters = true;
    feature_chain.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering = true;
    feature_chain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = true;


    std::vector<const char *> required_device_extension{vk::KHRSwapchainExtensionName};

    vk::DeviceCreateInfo device_create_info{};
    device_create_info.pNext = &feature_chain.get<vk::PhysicalDeviceFeatures2>();
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_create_infos.size());
    device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(required_device_extension.size());
    device_create_info.ppEnabledExtensionNames = required_device_extension.data();

    device = vk::raii::Device{physical_device, device_create_info};

    queues.graphics_queue = vk::raii::Queue{device, queues.graphics_index, 0};
    queues.transfer_queue = vk::raii::Queue{device, queues.transfer_index, 0};
    queues.compute_queue = vk::raii::Queue{device, queues.compute_index, 0};
}

void eruptor::hardware::Device::Create_alocator(Core& core)
{
    vma::AllocatorCreateInfo alocator_create_info{};
    alocator_create_info.physicalDevice = physical_device;

    alocator = vma::raii::Allocator{core.Get_instance_handle(), device, alocator_create_info};
}

bool eruptor::hardware::Device::Is_device_sutiable(const vk::raii::PhysicalDevice & device, Core & core)
{
    auto device_properties = device.getProperties();
    auto device_features = device.getFeatures();

    bool has_geometry_shader = device_features.geometryShader;
    std::vector<const char*> required_device_extension = {vk::KHRSwapchainExtensionName};

    auto available_device_extensions = device.enumerateDeviceExtensionProperties();
    bool supports_all_required_extensions =
    std::ranges::all_of( required_device_extension,
                         [&available_device_extensions]( auto const & required_device_extension )
                         {
                             return std::ranges::any_of( available_device_extensions,
                                                         [required_device_extension]( auto const & available_device_extensions )
                                                         { return strcmp( available_device_extensions.extensionName, required_device_extension ) == 0; } );
                         } );

    auto queue_families = device.getQueueFamilyProperties();
    bool supports_graphics =
    std::ranges::any_of(queue_families, [](auto const &qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

    bool supports_presentation{};
    uint32_t queueIndex = ~0;
    for (uint32_t qfpIndex = 0; qfpIndex < queue_families.size(); qfpIndex++)
    {
        if ((queue_families[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            device.getSurfaceSupportKHR(qfpIndex, core.Get_surface_handle())) supports_presentation = true;
    }

    auto features = device.template getFeatures2<vk::PhysicalDeviceFeatures2,
    vk::PhysicalDeviceVulkan11Features,
    vk::PhysicalDeviceVulkan13Features,
    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    bool supports_required_features = features.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy &&
    features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
    features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
    features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

    return has_geometry_shader && supports_all_required_extensions && supports_graphics && supports_required_features && supports_presentation;
}

std::vector<uint32_t> eruptor::hardware::Device::Queues::Get_unique_indices()
{
    std::vector<uint32_t> result{ graphics_index };
    if(transfer_index != graphics_index) result.push_back(transfer_index);
    if(compute_index != transfer_index) result.push_back(compute_index);

    return result;
}



