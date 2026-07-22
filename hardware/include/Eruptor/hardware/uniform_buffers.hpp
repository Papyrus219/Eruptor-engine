#ifndef ERUPTOR_HARDWARE_UNIFORM_BUFFERS_HPP
#define ERUPTOR_HARDWARE_UNIFORM_BUFFERS_HPP

#include <Eruptor/hardware/pipeline_id.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace eruptor::hardware
{

class Pipelines;
class Device;

struct Uniform_buffer_vp
{
    glm::mat4 view{};
    glm::mat4 proj{};
};

class Uniform_buffers
{
public:
    void Init(Device & device, const uint32_t MAX_FRAMES_IN_FLIGHTS);

    vk::raii::DescriptorSetLayout & Get_descriptor_set_layout() {return descriptor_set_layout;}

    void Bind_vp_buffer(vk::raii::CommandBuffer & command_buffer, uint32_t frame_index, Pipelines & pipeline, Pipeline_id pipeline_id, const glm::mat4 & view);
private:
    void Create_descriptor_set_layouts(const vk::raii::Device & device);
    void Create_uniform_buffers(vma::raii::Allocator & alocator, const uint32_t MAX_FRAMES_IN_FLIGHTS);
    void Create_descriptors_pool(const vk::raii::Device & device, const uint32_t MAX_FRAMES_IN_FLIGHTS);
    void Create_descriptors_sets(const vk::raii::Device & device, const uint32_t MAX_FRAMES_IN_FLIGHTS);

    vk::raii::DescriptorSetLayout descriptor_set_layout = nullptr;
    vk::raii::DescriptorPool descriptor_pool = nullptr;
    std::vector< vk::raii::DescriptorSet > descriptor_sets{};

    std::vector<vk::raii::Buffer> uniform_vp_buffers{};
    std::vector<void *> uniform_vp_buffers_mapped{};
};

}

#endif // ERUPTOR_HARDWARE_UNIFORM_BUFFERS_HPP
