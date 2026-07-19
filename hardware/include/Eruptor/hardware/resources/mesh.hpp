#ifndef ERUPTOR_HARDWARE_MESH_HPP
#define ERUPTOR_HARDWARE_MESH_HPP

#include <cstdint>

namespace eruptor::hardware
{

struct Mesh
{
    uint32_t vertex_offset{};
    uint32_t indices_offset{};

    uint32_t vertex_amount{};
    uint32_t indices_amount{};

    uint32_t material_id{};
};

}

#endif // ERUPTOR_HARDWARE_MESH_HPP
