#ifndef ERUPTOR_HARDWARE_RESOURCE_MANAGER_HPP
#define ERUPTOR_HARDWARE_RESOURCE_MANAGER_HPP

#include <Eruptor/hardware/resources/mesh.hpp>
#include <Eruptor/hardware/resources/texture.hpp>
#include <Eruptor/hardware/resources/geometry_buffer.hpp>
#include <Eruptor/hardware/resources/vertex.hpp>
#include <Eruptor/hardware/resources/primitive_meshes.hpp>
#include <Eruptor/hardware/constants.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>
#include <vector>
#include <array>
#include <utility>

namespace eruptor::hardware
{

class Command_manager;
class Device;
struct Mesh_data;
struct Texture_data;

class Resource_manager
{
public:
    void Init(vma::raii::Allocator & allocator, vk::DeviceSize max_texture_buffor_size, vk::DeviceSize max_vertex_buffor_size, vk::DeviceSize max_index_buffor_size, vk::DeviceSize max_text_buffer_size);

    void Assign_command_manager(Command_manager & command_manager);
    void Assign_device(Device & device);

    Mesh Get_mesh(uint32_t mesh_index);
    vk::ImageView Get_texture_view(uint32_t texture_index);
    vk::raii::Sampler & Get_texture_sampler() {return texture_sampler;};

    vk::raii::DescriptorSetLayout & Get_texture_set_layout() { return texture_set_layout; }
    vk::raii::DescriptorSet & Get_texture_descriptor_set(uint32_t texture_index) { return textures[texture_index].descriptor_set; }

    uint32_t Stage_mesh_data(Mesh_data & mesh_data);
    uint32_t Stage_texture_data(Texture_data & texture_data);

    void Start_staging_text(uint32_t current_frame);
    uint32_t Stage_text_data(const std::vector<Text_vertex> & vertices, uint32_t current_frame);

    void Bind_geometry_buffer(vk::raii::CommandBuffer & command_buffer);
    void Bind_text_buffer(vk::raii::CommandBuffer & command_buffer, uint32_t current_frame);

    void Upload_data_to_GPU();
    void Free_data_on_GPU();

private:
    static constexpr uint32_t MAX_TEXTURES{256};

    void Stage_mesh_primitives();

    vk::raii::Sampler texture_sampler = nullptr;
    vk::raii::DescriptorPool texture_descriptor_pool = nullptr;
    vk::raii::DescriptorSetLayout texture_set_layout = nullptr;

    Geometry_buffer geometry_buffer{};

    vma::raii::Buffer geometry_staging_buffer = nullptr;
    vma::raii::Buffer texture_stage_buffer = nullptr;

    std::array<vma::raii::Buffer, MAX_FRAMES_IN_FLIGHT> text_buffers{nullptr, nullptr};

    std::vector<Mesh> meshes{};
    std::vector<Texture> textures{};

    std::array<Mesh, std::to_underlying( Primitive_meshes_type::COUNT )> primitive_meshes{};

    void * geometry_stage_mapped_data{};
    void * texture_stage_mapped_memory{};

    void * vertex_buffer_mapped_memory{};
    void * index_buffer_mapped_memory{};

    std::array<void *, MAX_FRAMES_IN_FLIGHT> text_mapped_memories{};

    vk::raii::Fence upload_complete_fence = nullptr;
    vk::raii::Semaphore transpose_complete_semafore = nullptr;

    vk::DeviceSize max_texture_buffor_size{};
    vk::DeviceSize max_vertex_buffor_size{};
    vk::DeviceSize max_index_buffor_size{};
    vk::DeviceSize max_text_buffor_size{};

    vk::DeviceSize curr_stage_vertex_offset{};
    vk::DeviceSize curr_stage_index_offset{};
    vk::DeviceSize curr_stage_texture_offset{};

    vk::DeviceSize curr_gpu_vertex_offset{};
    vk::DeviceSize curr_gpu_index_offset{};
    vk::DeviceSize curr_gpu_texture_offset{};

    vk::DeviceSize stable_vertex_buffor_offset{};
    vk::DeviceSize stable_index_buffor_offset{};
    vk::DeviceSize stable_texture_buffor_offset{};

    std::array<vk::DeviceSize, MAX_FRAMES_IN_FLIGHT> curr_text_buffer_offsets{};

    size_t uploded_textures_count{};

    Device * device;
    Command_manager * command_manager;
};

struct Mesh_data
{
    void Clear();

    std::vector<Opaque_vertex> vertecies{};
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
