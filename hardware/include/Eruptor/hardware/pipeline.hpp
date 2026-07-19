#ifndef ERUPTOR_HARDWARE_PIPELINE_HPP
#define ERUPTOR_HARDWARE_PIPELINE_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <filesystem>
#include <vector>

namespace eruptor::hardware
{

class Device;
class Swapchain;
class Uniform_buffers;
class Resource_manager;

class Pipeline
{
public:
    void Init(Device & device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager);

    vk::raii::Pipeline & Get_pipeline_handle() {return graphics_pipeline;}
    vk::raii::PipelineLayout & Get_pipeline_layout() {return pipeline_layout;};
private:
    void Create_graphics_pipeline(Device & device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager);
    [[nodiscard]] vk::raii::ShaderModule Create_shader_module(Device & device, const std::vector<char>& code) const;

    static std::vector<char> Read_file(const std::filesystem::path & path);

    vk::raii::Pipeline graphics_pipeline = nullptr;
    vk::raii::PipelineLayout pipeline_layout = nullptr;
};

}

#endif // ERUPTOR_HARDWARE_PIPELINE_HPP
