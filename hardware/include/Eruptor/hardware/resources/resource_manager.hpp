#ifndef ERUPTOR_HARDWARE_RESOURCE_MANAGER_HPP
#define ERUPTOR_HARDWARE_RESOURCE_MANAGER_HPP

#include <Eruptor/hardware/resources/mesh.hpp>
#include <Eruptor/hardware/resources/texture.hpp>
#include <Eruptor/hardware/resources/geometry_buffer.hpp>
#include <Eruptor/hardware/resources/vertex.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>
#include <vector>

namespace eruptor::hardware
{

class Command_manager;
class Device;
struct Mesh_data;
struct Texture_data;

class Resource_manager
{
public:
    void Init(vma::raii::Allocator & allocator, vk::DeviceSize max_texture_buffor_size, vk::DeviceSize max_vertex_buffor_size, vk::DeviceSize max_index_buffor_size);

    void Assign_command_manager(Command_manager & command_manager);
    void Assign_device(Device & device);

    Mesh Get_mesh(uint32_t mesh_index);
    vk::ImageView Get_texture_view(uint32_t texture_index);
    vk::raii::Sampler & Get_texture_sampler() {return texture_sampler;};

    vk::raii::DescriptorSetLayout & Get_texture_set_layout() { return texture_set_layout; }
    vk::raii::DescriptorSet & Get_texture_descriptor_set(uint32_t texture_index) { return textures[texture_index].descriptor_set; }

    uint32_t Stage_mesh_data(Mesh_data & mesh_data);
    uint32_t Stage_texture_data(Texture_data & texture_data);

    void Bind_geometry_buffer(vk::raii::CommandBuffer & command_buffer);

    void Upload_data_to_GPU();

private:
    static constexpr uint32_t MAX_TEXTURES{256};

    vk::raii::Sampler texture_sampler = nullptr;
    vk::raii::DescriptorPool texture_descriptor_pool = nullptr;
    vk::raii::DescriptorSetLayout texture_set_layout = nullptr;

    Geometry_buffer geometry_buffer{};

    vma::raii::Buffer geometry_staging_buffer = nullptr;
    vma::raii::Buffer texture_stage_buffer = nullptr;

    std::vector<Mesh> meshes{};
    std::vector<Texture> textures{};

    void * geometry_stage_mapped_data{};
    void * texture_stage_mapped_memory{};

    void * vertex_buffer_mapped_memory{};
    void * index_buffer_mapped_memory{};

    vk::raii::Fence upload_complete_fence = nullptr;
    vk::raii::Semaphore transpose_complete_semafore = nullptr;

    vk::DeviceSize max_texture_buffor_size{};
    vk::DeviceSize max_vertex_buffor_size{};
    vk::DeviceSize max_index_buffor_size{};

    vk::DeviceSize curr_vertex_offset{};
    vk::DeviceSize curr_index_offset{};
    vk::DeviceSize curr_texture_offset{};

    Device * device;
    Command_manager * command_manager;
};

struct Mesh_data
{
    void Clear();

    std::vector<Vertex> vertecies{};
    std::vector<uint32_t> indices{};
    uint32_t material_id{};
};

struct Texture_data
{
    int width{};
    int height{};
    int tex_chanels{};
    unsigned char * pixels{};
    vk::Format format{};
};

}

#endif // ERUPTOR_HARDWARE_RESOURCE_MANAGER_HPP
