#ifndef ERUPTOR_RESOURCE_MATERIAL_HPP
#define ERUPTOR_RESOURCE_MATERIAL_HPP

#include <Eruptor/resource/resource_handle.hpp>

namespace eruptor::resource
{

struct Material
{
    Texture_handle diffuse_texture_handle{};
    Texture_handle specular_texture_handle{};
};

}

#endif //ERUPTOR_RESOURCE_MATERIAL_HPP
