#ifndef ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP
#define ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP

#include <Eruptor/resource/model.hpp>
#include <Eruptor/resource/material.hpp>
#include <Eruptor/resource/text_vertex_data.hpp>
#include <Eruptor/event/event_listener.hpp>
#include <Eruptor/physic/hitbox.hpp>
#include <assimp/material.h>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <filesystem>
#include <unordered_map>

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

struct Glyph
{
    glm::ivec2 size{};
    glm::ivec2 bearing{};
    uint32_t advance{};

    glm::vec2 uv_min{};
    glm::vec2 uv_max{};
};

struct Font_atlas
{
    std::filesystem::path path{};

    Status starus{ Status::UNINITIALIZED };

    Texture_handle texture_handle{};
    float size{};

    int width{512};
    int height{512};

    std::vector<unsigned char> bitmap{};
    std::unordered_map<char32_t, Glyph> glyphs{};
};

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
    physic::Hitbox Get_model_hitbox(Model_handle & model_handle);
    Material Get_material(Material_handle & material_handle);
    Font_atlas & Get_font_atlas(Font_handle & font_handle);

    Font_handle Add_font_atlas(const std::filesystem::path & path, float font_size);
    void Load_font_atlases();

    std::vector<Text_vertex_data> Generate_text_vertices_data(std::string_view text, float start_x, float start_y, Font_handle font_handle, glm::u8vec4 color);

    ///@todo Implenet text functions

    void Add_model_alias(uint32_t model_id, const std::string & model_alias);
    std::string_view Get_model_alias(uint32_t model_id);

    Model_handle Add_model(const std::filesystem::path & path);
    void Load_models();

    void On_event(const event::Event & event) override;

private:
    void Load_model(Model & model);
    void Load_font(Font_atlas & font_atlas);

    void Process_node(aiNode * node, const aiScene * scene, Model & model, const std::filesystem::path & directory, std::vector<glm::vec3> & all_vertecies);
    void Process_mesh(aiMesh * mesh, const aiScene * scene, Model & model, const std::filesystem::path & directory, std::vector<glm::vec3> & all_vertecies);
    Texture_handle Load_material_texture(aiMaterial * mat, aiTextureType ai_type, Texture_type type, const std::filesystem::path & directory);

    void Calculate_model_hitbox(Model & model, std::vector<glm::vec3> & all_vertecies);

    void Calculate_sphere_hitbox(physic::Sphere_hitbox & sphere, std::vector<glm::vec3> & all_vertecies);
    void Calculate_obb_hitbox(physic::OBB_hitbox & obb, std::vector<glm::vec3> & all_vertecies);
    void Calculate_capsule_hitbox(physic::Capsule_hitbox & capsule, std::vector<glm::vec3> & all_vertecies);

    glm::mat3 Compute_covariance(const std::vector<glm::vec3> & all_vertecies, glm::vec3 & centroid);
    glm::mat3 Jacobi_eigenvectors(glm::mat3 & cov, size_t iterations = 20);

    std::vector<Model> models{};
    std::vector<physic::AABB> models_AABB{};
    std::vector<physic::Hitbox> models_hitboxes{};
    std::vector<Material> materials{};
    std::vector<Texture_handle> textures_handles{};
    std::vector<Mesh_handle> mesh_handles{};
    std::vector<Font_atlas> fonts_atlases{};

    std::unordered_map<uint32_t, std::string> models_aliases{};

    FT_Library free_type{};

    event::Event_manager & event_manager;
    hardware::Resource_manager * hw_resource_manager{};
};

}

#endif // ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP
