#ifndef ERUPTOR_HARDWARE_PIPELINE_HPP
#define ERUPTOR_HARDWARE_PIPELINE_HPP

#include <Eruptor/hardware/pipeline_id.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <filesystem>
#include <vector>
#include <utility>
#include <optional>

namespace eruptor::hardware
{

class Device;
class Swapchain;
class Uniform_buffers;
class Resource_manager;

class Pipelines
{
public:
    void Init(Device & device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager);

    vk::raii::Pipeline & Get_pipeline_handle(Pipeline_id id);
    vk::raii::PipelineLayout & Get_pipeline_layout(Pipeline_id id);
private:
    void Create_graphics_pipelines(Device & device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager);
    [[nodiscard]] vk::raii::ShaderModule Create_shader_module(Device & device, const std::vector<char>& code) const;

    static std::vector<char> Read_file(const std::filesystem::path & path);

    std::array<std::optional<vk::raii::Pipeline>, std::to_underlying(Pipeline_id::COUNT)> pipelines{};
    std::array<std::optional<vk::raii::PipelineLayout>, std::to_underlying(Pipeline_id::COUNT)> pipeline_layouts{};
};

}

#endif // ERUPTOR_HARDWARE_PIPELINE_HPP
