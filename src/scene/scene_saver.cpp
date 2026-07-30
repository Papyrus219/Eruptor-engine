#include <Eruptor/scene/scene_saver.hpp>
#include <fstream>
#include <set>

std::string_view const eruptor::scene::Scene_saver::error_file{"Scene saver: Failed to open file to save to!"};

std::expected<void, std::string_view> eruptor::scene::Scene_saver::Save_scene_data(const Scene & scene, const std::filesystem::path & path, File_version file_version)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file{path};
    if(!file)
    {
        return std::unexpected{error_file};
    }

    std::set<uint32_t> models_id{};
    for(const auto & object : scene.render_objects)
    {
        models_id.insert( object.Get_model_handle().Get_id());
    }

    std::println(file, "Version: {}", Get_string_from_file_version(file_version));

    for(auto model_id : models_id)
    {
        resource::Model_handle model_handle{ model_id };

        std::println(file, "[{} = {}]", resource_manager->Get_model_alias(model_id), resource_manager->Get_model( model_handle ).path.c_str());

        if(file_version >= File_version::V1_1)
        {
            std::println(file, "Hitbox: {}", Get_string_from_hitbox_type( resource_manager->Get_model( model_handle ).hitbox_type ));
        }
        std::println(file, "");
    }

    std::println(file, "~~~~~");
    std::println(file, "");

    for(auto & [alias, id] : scene.objects_aliases)
    {
        auto & render_object = scene.render_objects[id];
        std::println(file, "<{}>", alias);
        std::println(file, "Model: {}", resource_manager->Get_model_alias( render_object.Get_model_handle() ));
        std::println(file, "Position: {} {} {}", render_object.Get_position().x, render_object.Get_position().y, render_object.Get_position().z);

        glm::vec3 e = glm::eulerAngles(render_object.Get_rotaion());
        if(std::abs(std::abs(e.x) - glm::pi<float>()) < 0.01f && std::abs(std::abs(e.z) - glm::pi<float>()) < 0.01f)
        {
            e.x = 0.0f;
            e.y = glm::pi<float>() - e.y;
            e.z = 0.0f;
        }
        std::println(file, "Rotation: {} {} {}", e.x, e.y, e.z);
        std::println(file, "Scale: {} {} {}", render_object.Get_scale().x, render_object.Get_scale().y, render_object.Get_scale().z);

        if(file_version >= File_version::V1_2)
        {
            std::println(file, "Color: {} {} {}", render_object.color.red, render_object.color.green, render_object.color.blue);
        }

        std::println(file, "");
    }

    return {};
}

std::string_view eruptor::scene::Scene_saver::Get_string_from_file_version(File_version file_version)
{
    switch(file_version)
    {
        case File_version::V1_0:
            return "1_0";
        case File_version::V1_1:
            return "1_1";
        case File_version::V1_2:
            return "1_2";
    }

    return "";
}

std::string_view eruptor::scene::Scene_saver::Get_string_from_hitbox_type(resource::Hitbox_type hitbox_type)
{
    switch(hitbox_type)
    {
        case resource::Hitbox_type::OBB:
            return "OBB";
        case resource::Hitbox_type::SPHERE:
            return "SPHERE";
        case resource::Hitbox_type::CAPSULE:
            return "CAPSULE";
    }

    return "";
}
