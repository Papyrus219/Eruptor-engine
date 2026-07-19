#include <Eruptor/hardware/resources/resource_manager.hpp>
#include <Eruptor/hardware/command_manager.hpp>
#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/utilities.hpp>

void eruptor::hardware::Resource_manager::Init(vma::raii::Allocator & allocator, vk::DeviceSize max_texture_buffor_size, vk::DeviceSize max_vertex_buffor_size, vk::DeviceSize max_index_buffor_size)
{
    this->max_texture_buffor_size = max_texture_buffor_size;
    this->max_vertex_buffor_size = max_vertex_buffor_size;
    this->max_index_buffor_size = max_index_buffor_size;

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
    sampler_info.setAddressModeU( vk::SamplerAddressMode::eRepeat );
    sampler_info.setAddressModeV( vk::SamplerAddressMode::eRepeat );
    sampler_info.setAddressModeW( vk::SamplerAddressMode::eRepeat );
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
    if(curr_vertex_offset + (mesh_data.vertecies.size() * sizeof(Vertex)) > max_vertex_buffor_size)
    {
        throw std::runtime_error{"ERROR::RESOURCE_MANAGER::Vertex stage buffer overflow."};
    }
    if(curr_index_offset + (mesh_data.indices.size() * sizeof(uint32_t)) > max_index_buffor_size)
    {
        throw std::runtime_error{"ERROR::RESOURCE_MANAGER::Index stage buffer overflow."};
    }

    memcpy(reinterpret_cast<char *>(geometry_stage_mapped_data) + curr_vertex_offset, mesh_data.vertecies.data(), sizeof(Vertex) * mesh_data.vertecies.size() );
    memcpy(reinterpret_cast<char *>(geometry_stage_mapped_data) + max_vertex_buffor_size + curr_index_offset, mesh_data.indices.data(), sizeof(uint32_t) * mesh_data.indices.size());

    Mesh tmp_mesh{};
    tmp_mesh.vertex_offset = curr_vertex_offset / sizeof(Vertex);
    tmp_mesh.indices_offset = curr_index_offset / sizeof(uint32_t);
    tmp_mesh.indices_amount = mesh_data.indices.size();
    tmp_mesh.vertex_amount = mesh_data.vertecies.size();
    tmp_mesh.material_id = mesh_data.material_id;

    meshes.push_back( std::move(tmp_mesh) );

    curr_vertex_offset += (mesh_data.vertecies.size() * sizeof(Vertex));
    curr_index_offset += (mesh_data.indices.size() * sizeof(uint32_t));

    return meshes.size() - 1;
}

uint32_t eruptor::hardware::Resource_manager::Stage_texture_data(Texture_data & texture_data)
{
    vk::DeviceSize image_size = texture_data.width * texture_data.height * texture_data.tex_chanels;

    vk::DeviceSize alligned_size = (image_size + 3) & ~3;

    if(curr_texture_offset + alligned_size > max_texture_buffor_size)
    {
        throw std::runtime_error{"ERROR::RESOURCE_MANAGER::Texture stage buffer overflow."};
    }

    memcpy(reinterpret_cast<char *>(texture_stage_mapped_memory) + curr_texture_offset, texture_data.pixels, image_size);

    Texture tmp_tex{};
    tmp_tex.Init(*device, texture_data.width, texture_data.height, texture_data.format, curr_texture_offset);
    tmp_tex.Create_descriptor_set(*device, texture_descriptor_pool, texture_set_layout, texture_sampler);
    tmp_tex.offset_in_stage_buffer = curr_texture_offset;
    tmp_tex.image_size = image_size;
    tmp_tex.width = texture_data.width;
    tmp_tex.height = texture_data.height;

    textures.push_back( std::move(tmp_tex) );

    curr_texture_offset += alligned_size;

    return textures.size() - 1;
}

void eruptor::hardware::Resource_manager::Bind_geometry_buffer(vk::raii::CommandBuffer & command_buffer)
{
    command_buffer.bindVertexBuffers(0, *geometry_buffer.vertex_buffer, {0});
    command_buffer.bindIndexBuffer(*geometry_buffer.index_buffer, 0, vk::IndexType::eUint32);
}

void eruptor::hardware::Resource_manager::Upload_data_to_GPU()
{
    device->Get_device_handle().resetFences({upload_complete_fence});

    auto & transpose_command_buffer = command_manager->Begin_transfer_command_record();

    std::vector<vk::ImageMemoryBarrier> transpose_bariers{};
    transpose_bariers.reserve( textures.size() );

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

    for(auto & tex : textures)
    {
        tmp_barrier.image = tex.texture_image;
        transpose_bariers.push_back( tmp_barrier );
    }

    transpose_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, nullptr, nullptr, transpose_bariers);
    transpose_bariers.clear();

    transpose_command_buffer.copyBuffer(*geometry_staging_buffer, *geometry_buffer.vertex_buffer, vk::BufferCopy{0, 0, curr_vertex_offset});
    transpose_command_buffer.copyBuffer(*geometry_staging_buffer, *geometry_buffer.index_buffer, vk::BufferCopy{max_vertex_buffor_size, 0, curr_index_offset});

    vk::BufferImageCopy region{};
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = vk::Offset3D{0, 0, 0};

    for(auto & tex : textures)
    {
        region.bufferOffset = tex.offset_in_stage_buffer;
        region.imageExtent = vk::Extent3D{tex.width, tex.height, 1};

        transpose_command_buffer.copyBufferToImage(texture_stage_buffer, tex.texture_image, vk::ImageLayout::eTransferDstOptimal, region);
    }

    if(device->queues.Get_graphics_queue_index() == device->queues.Get_transfer_queue_index())
    {
        tmp_barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        tmp_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        tmp_barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
        tmp_barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
        tmp_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        tmp_barrier.subresourceRange.levelCount = 1;
        tmp_barrier.subresourceRange.layerCount = 1;
        tmp_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        tmp_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        for(auto & tex : textures)
        {
            tmp_barrier.image = tex.texture_image;
            transpose_bariers.push_back( tmp_barrier );
        }
        transpose_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transpose_bariers);

        command_manager->End_command_record(transpose_command_buffer);
        command_manager->Submit_transfer_commands( transpose_command_buffer, {}, {}, {}, upload_complete_fence );

        transpose_bariers.clear();
    }
    else
    {
        tmp_barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        tmp_barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        tmp_barrier.srcQueueFamilyIndex = device->queues.Get_transfer_queue_index();
        tmp_barrier.dstQueueFamilyIndex = device->queues.Get_graphics_queue_index();
        tmp_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        tmp_barrier.subresourceRange.levelCount = 1;
        tmp_barrier.subresourceRange.layerCount = 1;
        tmp_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        tmp_barrier.dstAccessMask = {};

        for(auto & tex : textures)
        {
            tmp_barrier.image = tex.texture_image;
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
        tmp_barrier.srcQueueFamilyIndex = device->queues.Get_transfer_queue_index();
        tmp_barrier.dstQueueFamilyIndex = device->queues.Get_graphics_queue_index();
        tmp_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        tmp_barrier.subresourceRange.levelCount = 1;
        tmp_barrier.subresourceRange.layerCount = 1;
        tmp_barrier.srcAccessMask = {};
        tmp_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        for(auto & tex : textures)
        {
            tmp_barrier.image = tex.texture_image;
            transpose_bariers.push_back( tmp_barrier );
        }
        graphic_command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eFragmentShader, {}, nullptr, nullptr, transpose_bariers);

        command_manager->End_command_record(graphic_command_buffer);
        command_manager->Submit_graphic_commands(graphic_command_buffer, std::vector<vk::PipelineStageFlags>{vk::PipelineStageFlagBits::eFragmentShader}, semafores, {}, upload_complete_fence);
    }

    auto result = device->Get_device_handle().waitForFences({upload_complete_fence}, vk::True, UINT64_MAX);

    curr_index_offset = 0;
    curr_vertex_offset = 0;
    curr_texture_offset = 0;
}

void eruptor::hardware::Mesh_data::Clear()
{
    vertecies.clear();
    indices.clear();
}







