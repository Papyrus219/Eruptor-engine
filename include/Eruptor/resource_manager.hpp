#ifndef ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP
#define ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP

#include <Eruptor/resource/model.hpp>
#include <Eruptor/resource/material.hpp>
#include <Eruptor/event/event_listener.hpp>
#include <Eruptor/physic/colision_box.hpp>
#include <assimp/material.h>
#include <glm/glm.hpp>
#include <filesystem>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace eruptor::hardware
{
    class Resource_manager;
    struct Texture_data;
}

namespace eruptor::event
{
    class Event_manager;
}

namespace eruptor::resource
{

enum class Texture_type
{
    DIFFUSE,
    SPECULAR
};

class Resource_manager: public eruptor::event::Event_listener
{
public:
    Resource_manager();

    void Init(hardware::Resource_manager & hw_resource_manager);

    Model & Get_model(Model_handle & model_handle);
    physic::AABB Get_model_aabb(Model_handle & model_handle);
    Material Get_material(Material_handle & material_handle);

    Model_handle Add_model(const std::filesystem::path & path);
    void Load_models();

    void On_event(const event::Event & event) override;

private:
    void Load_model(Model & model);

    void Process_node(aiNode * node, const aiScene * scene, Model & model, const std::filesystem::path & directory, const glm::mat4 & parent_transform, physic::AABB & aabb);
    void Process_mesh(aiMesh * mesh, const aiScene * scene, Model & model, const std::filesystem::path & directory, const glm::mat4 & transform, physic::AABB & aabb);
    Texture_handle Load_material_texture(aiMaterial * mat, aiTextureType ai_type, Texture_type type, const std::filesystem::path & directory);

    std::vector<Model> models{};
    std::vector<physic::AABB> models_AABB;
    std::vector<Material> materials{};
    std::vector<Texture_handle> textures_handles{};
    std::vector<Mesh_handle> mesh_handles{};

    event::Event_manager & event_manager;

    hardware::Resource_manager * hw_resource_manager{};
};

}

#endif // ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP
