#include <Eruptor/hardware/resources/resource_manager.hpp>
#include <Eruptor/hardware/command_manager.hpp>
#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/utilities.hpp>

void eruptor::hardware::Resource_manager::Init(vma::raii::Allocator & allocator, vk::DeviceSize max_texture_buffor_size, vk::DeviceSize max_vertex_buffor_size, vk::DeviceSize max_index_buffor_size, vk::DeviceSize max_text_buffor_size)
{
    this->max_texture_buffor_size = max_texture_buffor_size;
    this->max_vertex_buffor_size = max_vertex_buffor_size;
    this->max_index_buffor_size = max_index_buffor_size;
    this->max_text_buffor_size = max_text_buffor_size;

    vk::BufferCreateInfo stage_buffer_info{};
    stage_buffer_info.usage = vk::BufferUsageFlagBits::eTransferSrc;
    stage_buffer_info.size = max_texture_buffor_size;
    stage_buffer_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo stage_alloc_info{};
    stage_alloc_info.usage = vma::MemoryUsage::eAuto;
    stage_alloc_info.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped;

    vma::AllocationInfo stage_alloc_result{};

    texture_stage_buffer = allocator.createBuffer(stage_buffer_info, stage_alloc_info, stage_alloc_result);
    texture_stage_mapped_memory = stage_alloc_result.pMappedData;

    stage_buffer_info.size = max_vertex_buffor_size + max_index_buffor_size;

    geometry_staging_buffer = allocator.createBuffer(stage_buffer_info, stage_alloc_info, stage_alloc_result);
    geometry_stage_mapped_data = stage_alloc_result.pMappedData;

    vk::BufferCreateInfo vertex_create_info{};
    vertex_create_info.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    vertex_create_info.size = max_vertex_buffor_size;
    vertex_create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo vertex_alocation_info{};
    vertex_alocation_info.usage = vma::MemoryUsage::eAuto;

    vk::BufferCreateInfo index_create_info{};
    index_create_info.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    index_create_info.size = max_index_buffor_size;
    index_create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationCreateInfo index_alocation_info{};
    index_alocation_info.usage = vma::MemoryUsage::eAuto;

    vma::AllocationCreateInfo text_alocation_info{};
    text_alocation_info.usage = vma::MemoryUsage::eAuto;
    text_alocation_info.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped;

    vk::BufferCreateInfo text_create_info{};
    text_create_info.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    text_create_info.size = max_text_buffor_size;
    text_create_info.sharingMode = vk::SharingMode::eExclusive;

    vma::AllocationInfo text_alloc_result{};

    for(auto i{0UZ}; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        text_buffers[i] = allocator.createBuffer(text_create_info, text_alocation_info, text_alloc_result);
        text_mapped_memories[i] = text_alloc_result.pMappedData;
    }

    geometry_buffer.vertex_buffer = allocator.createBuffer(vertex_create_info, vertex_alocation_info);
    geometry_buffer.index_buffer = allocator.createBuffer(index_create_info, index_alocation_info);

    vk::SemaphoreCreateInfo semaphore_info{};
    transpose_complete_semafore = vk::raii::Semaphore(device->Get_device_handle(), semaphore_info);

    vk::FenceCreateInfo fence_info{};
    upload_complete_fence = vk::raii::Fence(device->Get_device_handle(), fence_info);

    vk::PhysicalDeviceProperties properties = device->Get_physical_device_handle().getProperties();

    vk::SamplerCreateInfo sampler_info{};
    sampler_info.setMagFilter( vk::Filter::eLinear );
    sampler_info.setMinFilter( vk::Filter::eLinear );
    sampler_info.setMipmapMode( vk::SamplerMipmapMode::eLinear );
    sampler_info.setMipLodBias( 0.0f );
    sampler_info.setMinLod( 0.0f );
    sampler_info.setMaxLod( 0.0f );
    sampler_info.setAddressModeU( vk::SamplerAddressMode::eClampToEdge );
    sampler_info.setAddressModeV( vk::SamplerAddressMode::eClampToEdge );
    sampler_info.setAddressModeW( vk::SamplerAddressMode::eClampToEdge );
    sampler_info.setAnisotropyEnable( vk::True );
    sampler_info.setMaxAnisotropy( properties.limits.maxSamplerAnisotropy );
    sampler_info.setCompareEnable( vk::False );
    sampler_info.setCompareOp( vk::CompareOp::eAlways );
    sampler_info.setBorderColor( vk::BorderColor::eIntOpaqueBlack );
    sampler_info.setUnnormalizedCoordinates( vk::False );

    texture_sampler = vk::raii::Sampler{device->Get_device_handle(), sampler_info};

    vk::DescriptorSetLayoutBinding sampler_binding{};
    sampler_binding.setBinding( 0 );
    sampler_binding.setDescriptorType( vk::DescriptorType::eCombinedImageSampler );
    sampler_binding.setDescriptorCount( 1 );
    sampler_binding.setStageFlags( vk::ShaderStageFlagBits::eFragment );

    vk::DescriptorSetLayoutCreateInfo layout_info{};
    layout_info.setBindings( sampler_binding );
    texture_set_layout = vk::raii::DescriptorSetLayout{ device->Get_device_handle(), layout_info };

    vk::DescriptorPoolSize pool_size{};
    pool_size.setType( vk::DescriptorType::eCombinedImageSampler );
    pool_size.setDescriptorCount( MAX_TEXTURES );

    vk::DescriptorPoolCreateInfo pool_info{};
    pool_info.setFlags( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet );
    pool_info.setMaxSets( MAX_TEXTURES );
    pool_info.setPoolSizes( pool_size );

    texture_descriptor_pool = vk::raii::DescriptorPool{device->Get_device_handle(), pool_info};

    Stage_mesh_primitives();
}

void eruptor::hardware::Resource_manager::Assign_command_manager(Command_manager & command_manager)
{
    this->command_manager = &command_manager;
}

void eruptor::hardware::Resource_manager::Assign_device(Device & device)
{
    this->device = &device;
}

eruptor::hardware::Mesh eruptor::hardware::Resource_manager::Get_mesh(uint32_t mesh_index)
{
    return meshes[ mesh_index ];
}

vk::ImageView eruptor::hardware::Resource_manager::Get_texture_view(uint32_t texture_index)
{
    return *textures[ texture_index ].texture_view;
}

uint32_t eruptor::hardware::Resource_manager::Stage_mesh_data(Mesh_data & mesh_data)
{
    if(curr_stage_vertex_offset + (mesh_data.vertecies.size() * sizeof(Opaque_vertex)) > max_vertex_buffor_size)
    {
        throw std::runtime_error{"ERROR::RESOURCE_MANAGER::Vertex stage buffer overflow."};
    }
    if(curr_stage_index_offset + (mesh_data.indices.size() * sizeof(uint32_t)) > max_index_buffor_size)
    {
        throw std::runtime_error{"ERROR::RESOURCE_MANAGER::Index stage buffer overflow."};
    }

    memcpy(reinterpret_cast<char *>(geometry_stage_mapped_data) + curr_stage_vertex_offset, mesh_data.vertecies.data(), sizeof(Opaque_vertex) * mesh_data.vertecies.size() );
    memcpy(reinterpret_cast<char *>(geometry_stage_mapped_data) + max_vertex_buffor_size + curr_stage_index_offset, mesh_data.indices.data(), sizeof(uint32_t) * mesh_data.indices.size());

    Mesh tmp_mesh{};
    tmp_mesh.vertex_offset = (curr_gpu_vertex_offset + curr_stage_vertex_offset) / sizeof(Opaque_vertex);
    tmp_mesh.indices_offset = (curr_gpu_index_offset + curr_stage_index_offset) / sizeof(uint32_t);
    tmp_mesh.indices_amount = mesh_data.indices.size();
    tmp_mesh.material_id = mesh_data.material_id;

    meshes.push_back( std::move(tmp_mesh) );

    curr_stage_vertex_offset += (mesh_data.vertecies.size() * sizeof(Opaque_vertex));
    curr_stage_index_offset += (mesh_data.indices.size() * sizeof(uint32_t));

    return meshes.size() - 1;
}

uint32_t eruptor::hardware::Resource_manager::Stage_texture_data(Texture_data & texture_data)
{
    vk::DeviceSize image_size = texture_data.width * texture_data.height * texture_data.tex_chanels;

    vk::DeviceSize alligned_size = (image_size + 3) & ~3;

    if(curr_stage_texture_offset + alligned_size > max_texture_buffor_size)
    {
        throw std::runtime_error{"ERROR::RESOURCE_MANAGER::Texture stage buffer overflow."};
    }

    memcpy(reinterpret_cast<char *>(texture_stage_mapped_memory) + curr_stage_texture_offset, texture_data.pixels, image_size);

    Texture tmp_tex{};
    tmp_tex.Init(*device, texture_data.width, texture_data.height, texture_data.format, curr_stage_texture_offset);
    tmp_tex.Create_descriptor_set(*device, texture_descriptor_pool, texture_set_layout, texture_sampler);
    tmp_tex.offset_in_stage_buffer = curr_stage_texture_offset;
    tmp_tex.image_size = image_size;
    tmp_tex.width = texture_data.width;
    tmp_tex.height = texture_data.height;

    textures.push_back( std::move(tmp_tex) );

    curr_stage_texture_offset += alligned_size;

    return textures.size() - 1;
}

void eruptor::hardware::Resource_manager::Start_staging_text(uint32_t current_frame)
{
    curr_text_buffer_offsets[ current_frame ] = 0;
}

uint32_t eruptor::hardware::Resource_manager::Stage_text_data(const std::vector<Text_vertex> & vertices, uint32_t current_frame)
{
    vk::DeviceSize data_size = vertices.size() * sizeof( Text_vertex );

    if(curr_text_buffer_offsets[ current_frame ] + data_size > max_text_buffor_size)
    {
        throw std::runtime_error{"ERUPTOR::HARDWARE::RESOURCE_MANAGER::Text buffer overflow."};
    }

    memcpy(static_cast<char*>(text_mapped_memories[ current_frame ]) + curr_text_buffer_offsets[ current_frame ], vertices.data(), data_size);

    uint32_t first_vertex = curr_text_buffer_offsets[ current_frame ] / sizeof( Text_vertex );
    curr_text_buffer_offsets[ current_frame ] += data_size;

    return first_vertex;
}

void eruptor::hardware::Resource_manager::Bind_geometry_buffer(vk::raii::CommandBuffer & command_buffer)
{
    command_buffer.bindVertexBuffers(0, *geometry_buffer.vertex_buffer, {0});
    command_buffer.bindIndexBuffer(*geometry_buffer.index_buffer, 0, vk::IndexType::eUint32);
}

void eruptor::hardware::Resource_manager::Bind_text_buffer(vk::raii::CommandBuffer & command_buffer, uint32_t current_frame)
{
    command_buffer.bindVertexBuffers(0, *text_buffers[ current_frame ], {0});
}

void eruptor::hardware::Resource_manager::Upload_data_to_GPU()
{
    device->Get_device_handle().resetFences({upload_complete_fence});

    auto & transpose_command_buffer = command_manager->Begin_transfer_command_record();

    if (curr_stage_vertex_offset > 0)
    {
        transpose_command_buffer.copyBuffer(*geometry_staging_buffer, *geometry_buffer.vertex_buffer, vk::BufferCopy{0, curr_gpu_vertex_offset, curr_stage_vertex_offset});
    }
    if (curr_stage_index_offset > 0)
    {
        transpose_command_buffer.copyBuffer(*geometry_staging_buffer, *geometry_buffer.index_buffer, vk::BufferCopy{max_vertex_buffor_size, curr_gpu_index_offset, curr_stage_index_offset});
    }

    if (uploded_textures_count < textures.size())
    {
        std::vector<vk::ImageMemoryBarrier> transpose_bariers{};
        transpose_bariers.reserve(textures.size() - uploded_textures_count);

        vk::ImageMemoryBarrier tmp_barrier{};
        tmp_barrier.oldLayout = vk::ImageLayout::eUndefined;
        tmp_barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
        tmp_barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
        tmp_barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
        tmp_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        tmp_barrier.subresourceRange.levelCount = 1;
        tmp_barrier.subresourceRange.layerCount = 1;
        tmp_barrier.srcAccessMask = {};
        tmp_barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        for(auto i = uploded_textures_count; i < textures.size(); i++)
        {
            tmp_barrier.image = textures[i].texture_image;
            transpose_bariers.push_back( tmp_barrier );
        }

        transpose_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, nullptr, transpose_bariers);
        transpose_bariers.clear();

        vk::BufferImageCopy region{};
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = vk::Offset3D{0, 0, 0};

        for(size_t i = uploded_textures_count; i < textures.size(); ++i)
        {
            auto & tex = textures[i];
            region.bufferOffset = tex.offset_in_stage_buffer;
            region.imageExtent = vk::Extent3D{tex.width, tex.height, 1};

            transpose_command_buffer.copyBufferToImage(texture_stage_buffer, tex.texture_image, vk::ImageLayout::eTransferDstOptimal, region);
        }

        if(device->queues.Get_graphics_queue_index() == device->queues.Get_transfer_queue_index())
        {
            tmp_barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            tmp_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            tmp_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            tmp_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            for(size_t i = uploded_textures_count; i < textures.size(); ++i)
            {
                tmp_barrier.image = textures[i].texture_image;
                transpose_bariers.push_back( tmp_barrier );
            }
            transpose_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transpose_bariers);

            command_manager->End_command_record(transpose_command_buffer);
            command_manager->Submit_transfer_commands( transpose_command_buffer, {}, {}, {}, upload_complete_fence );
        }
        else
        {
            tmp_barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            tmp_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            tmp_barrier.srcQueueFamilyIndex = device->queues.Get_transfer_queue_index();
            tmp_barrier.dstQueueFamilyIndex = device->queues.Get_graphics_queue_index();
            tmp_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            tmp_barrier.dstAccessMask = {};

            for(size_t i = uploded_textures_count; i < textures.size(); ++i)
            {
                tmp_barrier.image = textures[i].texture_image;
                transpose_bariers.push_back( tmp_barrier );
            }
            transpose_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eBottomOfPipe, {}, nullptr, nullptr, transpose_bariers);

            std::vector<vk::Semaphore> semafores{};
            semafores.push_back(transpose_complete_semafore);

            command_manager->End_command_record(transpose_command_buffer);
            command_manager->Submit_transfer_commands(transpose_command_buffer, {}, {}, semafores);
            transpose_bariers.clear();

            auto & graphic_command_buffer = command_manager->Begin_ownership_graphic_command_record();

            tmp_barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            tmp_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            tmp_barrier.srcAccessMask = {};
            tmp_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            for(size_t i = uploded_textures_count; i < textures.size(); ++i)
            {
                tmp_barrier.image = textures[i].texture_image;
                transpose_bariers.push_back( tmp_barrier );
            }
            graphic_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transpose_bariers);

            command_manager->End_command_record(graphic_command_buffer);
            command_manager->Submit_graphic_commands(graphic_command_buffer, std::vector<vk::PipelineStageFlags>{vk::PipelineStageFlagBits::eFragmentShader}, semafores, {}, upload_complete_fence);
        }

        uploded_textures_count = textures.size();
    }
    else
    {
        command_manager->End_command_record(transpose_command_buffer);
        command_manager->Submit_transfer_commands( transpose_command_buffer, {}, {}, {}, upload_complete_fence );
    }

    auto result = device->Get_device_handle().waitForFences({upload_complete_fence}, vk::True, UINT64_MAX);

    curr_gpu_index_offset += curr_stage_index_offset;
    curr_gpu_vertex_offset += curr_stage_vertex_offset;
    curr_gpu_texture_offset += curr_stage_texture_offset;

    curr_stage_index_offset = 0;
    curr_stage_vertex_offset = 0;
}

void eruptor::hardware::Resource_manager::Stage_mesh_primitives()
{
    std::vector<Opaque_vertex> vertecies =
    {
        {{-0.5f, -0.5f,  0.5f},   {0.0f,  0.0f,  1.0f},   {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f},   {0.0f,  0.0f,  1.0f},   {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f},   {0.0f,  0.0f,  1.0f},   {0.0f, 1.0f}},

        {{ 0.5f, -0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},   {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},   {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},   {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f},   {0.0f,  0.0f, -1.0f},   {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f},   {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f},   {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f},   {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f},   {0.0f, 1.0f}},

        {{ 0.5f, -0.5f,  0.5f},   {1.0f,  0.0f,  0.0f},   {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f},   {1.0f,  0.0f,  0.0f},   {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f},   {1.0f,  0.0f,  0.0f},   {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f},   {1.0f,  0.0f,  0.0f},   {0.0f, 1.0f}},

        {{-0.5f,  0.5f,  0.5f},   {0.0f,  1.0f,  0.0f},   {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f},   {0.0f,  1.0f,  0.0f},   {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f},   {0.0f,  1.0f,  0.0f},   {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f},   {0.0f,  1.0f,  0.0f},   {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f},   {0.0f, -1.0f,  0.0f},   {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f},   {0.0f, -1.0f,  0.0f},   {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f},   {0.0f, -1.0f,  0.0f},   {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f},   {0.0f, -1.0f,  0.0f},   {0.0f, 1.0f}},
    };

    std::vector<uint32_t> indices =
    {
         0,  1,  2,   2,  3,  0,
         4,  5,  6,   6,  7,  4,
         8,  9, 10,  10, 11,  8,
        12, 13, 14,  14, 15, 12,
        16, 17, 18,  18, 19, 16,
        20, 21 ,22,  22, 23, 20
    };

    Mesh cube_mesh{};

    memcpy(reinterpret_cast<char *>(geometry_stage_mapped_data) + curr_stage_vertex_offset, vertecies.data(), sizeof(Opaque_vertex) * vertecies.size() );
    memcpy(reinterpret_cast<char *>(geometry_stage_mapped_data) + max_vertex_buffor_size + curr_stage_index_offset, indices.data(), sizeof(uint32_t) * indices.size());

    cube_mesh.vertex_offset =  curr_stage_vertex_offset / sizeof(Opaque_vertex);
    cube_mesh.indices_offset = curr_stage_index_offset / sizeof(uint32_t);
    cube_mesh.indices_amount = indices.size();

    primitive_meshes[ std::to_underlying( Primitive_meshes_type::CUBE ) ] = cube_mesh;

    curr_stage_vertex_offset += (vertecies.size() * sizeof(Opaque_vertex));
    curr_stage_index_offset += (indices.size() * sizeof(uint32_t));

    vertecies.clear();
    indices.clear();

    stable_vertex_buffor_offset = curr_stage_vertex_offset;
    stable_index_buffor_offset = curr_stage_index_offset;

    Upload_data_to_GPU();
}

void eruptor::hardware::Resource_manager::Free_data_on_GPU()
{
    meshes.clear();
    textures.clear();

    curr_gpu_vertex_offset = stable_vertex_buffor_offset;
    curr_gpu_index_offset = stable_index_buffor_offset;
    curr_gpu_texture_offset = stable_texture_buffor_offset;
}

void eruptor::hardware::Mesh_data::Clear()
{
    vertecies.clear();
    indices.clear();
}







