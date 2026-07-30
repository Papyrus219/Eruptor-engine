#ifndef ERUPTOR_SCENE_SCENE_SAVER_HPP
#define ERUPTOR_SCENE_SCENE_SAVER_HPP

#include <Eruptor/resource_manager.hpp>
#include <Eruptor/scene/scene.hpp>
#include <filesystem>
#include <expected>

namespace eruptor::scene
{

class Scene_saver
{
public:
    enum class File_version
    {
        V1_0,
        V1_1,
        V1_2
    };

    void Assign_resource_manager(resource::Resource_manager & resource_manager) {this->resource_manager = &resource_manager;}
    std::expected<void, std::string_view> Save_scene_data(const Scene & scene, const std::filesystem::path & path, File_version file_version = File_version::V1_2);

private:
    std::string_view Get_string_from_file_version(File_version file_version);
    std::string_view Get_string_from_hitbox_type(resource::Hitbox_type hitbox_type);

    resource::Resource_manager * resource_manager{};

    static const std::string_view error_file;
};

}

#endif // ERUPTOR_SCENE_SCENE_SAVER_HPP
