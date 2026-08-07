#ifndef ERUPTOR_HARDWARE_PRIMITVE_MESHES_HPP
#define ERUPTOR_HARDWARE_PRIMITVE_MESHES_HPP

#include <cstdint>

namespace eruptor::hardware
{

enum class Primitive_meshes_type: uint8_t
{
    CUBE,
    SPHERE,
    COUNT
};

}

#endif //ERUPTOR_HARDWARE_PRIMITVE_MESHES_HPP
