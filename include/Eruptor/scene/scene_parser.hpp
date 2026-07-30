#ifndef ERUPTOR_SCENE_SCENE_PARSER_HPP
#define ERUPTOR_SCENE_SCENE_PARSER_HPP

#include <Eruptor/scene/scene.hpp>
#include <Eruptor/resource_manager.hpp>
#include <Eruptor/resource/resource_handle.hpp>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <string_view>
#include <expected>

namespace eruptor::scene
{

class Scene_parser
{
public:
    void Assign_resource_manager(resource::Resource_manager & resource_manager) {this->resource_manager = &resource_manager;}
    std::expected<Scene, std::string_view> Load_scene(const std::filesystem::path & scene_path);

private:
    void Load_file_to_buffor(const std::filesystem::path & scene_path);
    void Parse_line(std::string_view line, Scene & scene);
    std::expected<glm::vec3, std::string_view>  Parse_3_numbers(std::string_view numbers);

    resource::Resource_manager * resource_manager{};
    resource::Model * current_parsed_model{};

    std::unordered_map<std::string_view, std::pair<std::string_view, resource::Model_handle>> model_variables{};
    std::string buffor{};
    uint32_t line_count{};
    bool is_in_model_loading_stage{};

    enum class Line_mode
    {
        NONE,
        VERSION_CHECK,
        MODEL_NAME,
        MODEL_HITBOX,
        MODELS_DECLARATION_END,
        OBJECT_DECLARATION,
        OBJECT_MODEL,
        OBJECT_POSITION,
        OBJECT_ROTATION,
        OBJECT_SCALE,
        OBJECT_COLOR,
    } line_mode{};

    enum class Version
    {
        V1_0,
        V1_1,
        V1_2,
    } file_version;

    bool error_happen{};
    std::string_view error_message{};

    static const std::string error_file_load;
    static const std::string error_text_parsing;
    static const std::string error_numbers_parsing;
};

}

#endif // ERUPTOR_SCENE_SCENE_PARSER_HPP
