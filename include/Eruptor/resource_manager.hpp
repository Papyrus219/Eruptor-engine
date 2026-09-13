#ifndef ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP
#define ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP

#include <Eruptor/resource/scene_parser.hpp>
#include <Eruptor/resource/resource.hpp>
#include <Eruptor/resource/model.hpp>
#include <Eruptor/resource/material.hpp>
#include <Eruptor/resource/texture.hpp>
#include <Eruptor/resource/text_vertex_data.hpp>
#include <Eruptor/event/event_listener.hpp>
#include <Eruptor/physic/hitbox.hpp>
#include <assimp/material.h>
#include <glm/glm.hpp>
#include <filesystem>
#include <unordered_map>

struct FT_LibraryRec_;
using FT_Library = FT_LibraryRec_ *;

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

namespace eruptor::hardware
{
    class Resource_manager;
    struct Texture_data;
}

namespace eruptor::physic
{
    class Physic_manager;
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
    Resource_Texture_handle texture_handle{};
    float size{};

    int width{512};
    int height{512};

    std::vector<unsigned char> bitmap{};
    std::unordered_map<char32_t, Glyph> glyphs{};
};

class Resource_manager: public eruptor::event::Event_listener
{
public:
    Resource_manager();

    void Init(hardware::Resource_manager & hw_resource_manager, physic::Physic_manager & physic_manager);

    physic::Physic_manager & Get_assigned_physic_manager();

    Model & Get_model(Model_handle & model_handle, bool skip_assertion = false);
    Material Get_material(Material_handle & material_handle);
    Texture Get_texture(Resource_Texture_handle & texture_handle);
    Font_atlas & Get_font_atlas(Font_handle & font_handle);

    Model_handle Get_model_handle(std::string_view model_alias);
    const std::filesystem::path & Get_model_path(Model_handle & model_handle) const;

    Font_handle Add_font_atlas(const std::filesystem::path & path, float font_size);

    std::vector<Text_vertex_data> Generate_text_vertices_data(std::string_view text, float start_x, float start_y, Font_handle font_handle, glm::u8vec4 color);

    void Add_model_alias(uint32_t model_id, const std::string & model_alias);
    std::string_view Get_model_alias(uint32_t model_id);

    Model_handle Add_model(const std::filesystem::path & path);

    void Load_resources();

    void On_event(const event::Event & event) override;

    std::filesystem::path texture_dirr_path{};

    Scene_parser scene_parser{};

    virtual ~Resource_manager() override;

private:
    void Load_models();
    void Load_font_atlases();
    void Load_textures();

    void Load_model(uint32_t id, Resource<Model> & model_resource);
    void Load_font(Font_atlas & font_atlas, const std::filesystem::path & path);

    void Process_node(aiNode * node, const aiScene * scene, Model & model, const std::filesystem::path & directory, std::vector<glm::vec3> & all_vertecies);
    void Process_mesh(aiMesh * mesh, const aiScene * scene, Model & model, const std::filesystem::path & directory, std::vector<glm::vec3> & all_vertecies);
    Resource_Texture_handle Add_material_texture(aiMaterial * mat, aiTextureType ai_type, Texture_type type, const std::filesystem::path & directory);

    std::vector< Resource<Model> > models{};
    std::vector< Resource<Font_atlas> > fonts_atlases{};
    std::vector< Resource<Texture> > textures{};

    std::vector<Material> materials{};
    std::vector<Mesh_handle> mesh_handles{};

    std::unordered_map<uint32_t, std::string> models_aliases{};

    FT_Library free_type{};

    event::Event_manager & event_manager;
    hardware::Resource_manager * hw_resource_manager{};
    physic::Physic_manager * physic_manager{};
};

}

#endif // ERUPTOR_RESOURCE_RESOURCE_MANAGER_HPP
