#ifndef ERUPTOR_RESOURCE_MODEL_HPP
#define ERUPTOR_RESOURCE_MODEL_HPP

#include <Eruptor/resource/resource_handle.hpp>
#include <Eruptor/physic/hitbox_type.hpp>
#include <filesystem>
#include <vector>

namespace eruptor::resource
{

struct Model
{
    physic::Hitbox_type hitbox_type{};
    std::vector<Mesh_handle> Meshes_handles{};
    std::vector<Material_handle> materials_handles{};
};

}

#endif //ERUPTOR_RESOURCES_MODEL_HPP
