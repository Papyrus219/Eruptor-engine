#include <Eruptor/hardware/resources/texture.hpp>
#include <Eruptor/hardware/device.hpp>

void eruptor::hardware::Texture::Init(Device & device, int width, int height, vk::Format format, vk::DeviceSize offset_in_stage_buffer, Texture_type tex_type, uint8_t layer_count)
{
    this->format = format;
    this->layer_count = layer_count;
    this->offset_in_stage_buffer = offset_in_stage_buffer;

    vk::ImageCreateFlags image_create_flags = (tex_type == Texture_type::TYPE_2D)? vk::ImageCreateFlags{} : vk::ImageCreateFlagBits::eCubeCompatible;
    vk::ImageViewType  image_view_type = (tex_type == Texture_type::TYPE_2D)? vk::ImageViewType::e2D : vk::ImageViewType::eCube;

    texture_image = device.Create_image(width, height, format, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, image_create_flags, layer_count);
    texture_view = device.Create_image_view(texture_image, format, vk::ImageAspectFlagBits::eColor, image_view_type, layer_count);
}

void eruptor::hardware::Texture::Create_descriptor_set(Device & device, vk::raii::DescriptorPool & pool, const vk::raii::DescriptorSetLayout & layout, const vk::raii::Sampler & sampler)
{
    vk::DescriptorSetAllocateInfo alloc_info{};
    alloc_info.setDescriptorPool( pool );
    alloc_info.setSetLayouts( *layout );

    auto sets = device.Get_device_handle().allocateDescriptorSets( alloc_info );
    descriptor_set = std::move( sets[0] );

    vk::DescriptorImageInfo image_info{};
    image_info.setSampler( sampler );
    image_info.setImageView( texture_view );
    image_info.setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal );

    vk::WriteDescriptorSet write{};
    write.setDstSet( descriptor_set );
    write.setDstBinding( 0 );
    write.setDstArrayElement( 0 );
    write.setDescriptorCount( 1 );
    write.setDescriptorType( vk::DescriptorType::eCombinedImageSampler );
    write.setImageInfo( image_info );

    device.Get_device_handle().updateDescriptorSets( write, {} );
}
