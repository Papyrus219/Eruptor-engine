#ifndef ERUPTOR_RESOURCE_TEXTURE_HPP
#define ERUPTOR_RESOURCE_TEXTURE_HPP

#include <Eruptor/resource/resource_handle.hpp>

namespace eruptor::resource
{

enum class Texture_type
{
    DIFFUSE,
    SPECULAR,
    CUBEMAP
};

struct Texture
{
    Texture_type type{};

    Hardware_Texture_handle hw_tex_handle{};
};

}

#endif //ERUPTOR_RESOURCE_TEXTURE_HPP
