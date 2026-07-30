#include <Eruptor/scene/scene_parser.hpp>
#include <Eruptor/resource/colors.hpp>
#include <fstream>
#include <iostream>
#include <print>

#ifndef NDEBUG
constexpr bool Debug_mode = true;
#else
constexpr bool Debug_mode = false;
#endif

const std::string eruptor::scene::Scene_parser::error_file_load{"Scene parser: Failed to load file!"};
const std::string eruptor::scene::Scene_parser::error_text_parsing{"Scene parser: Failed to parse text correctly!"};
const std::string eruptor::scene::Scene_parser::error_numbers_parsing{"Scene parser: Failed to parse numbers correctly!"};

std::expected<eruptor::scene::Scene, std::string_view> eruptor::scene::Scene_parser::Load_scene(const std::filesystem::path & scene_path)
{
    Load_file_to_buffor(scene_path);
    if(error_happen) return std::unexpected{error_message};

    Scene scene{};

    std::string_view buffer_view = buffor;
    size_t pos{};
    while(pos < buffer_view.size())
    {
        size_t end = buffer_view.find('\n', pos);
        if(end == std::string_view::npos) end = buffer_view.size();
        line_count++;

        std::string_view line = buffer_view.substr(pos, end - pos);
        if(line.size() == 0) //If line empty = skip.
        {
            pos = end + 1;
            continue;
        }

        if(line_mode == Line_mode::NONE) //If none mode select it
        {
            switch(line[0])
            {
                case '#':                                          //Comment
                    pos = end + 1;
                    continue;
                case '[':
                    line_mode = Line_mode::MODEL_NAME;             //Model path declaration
                    break;
                case '<':
                    line_mode = Line_mode::OBJECT_DECLARATION;     //Object declaration
                    break;
                case '~':
                    line_mode = Line_mode::MODELS_DECLARATION_END; //End of models declaration in file == Time to load
                    break;
                default:
                    line_mode = Line_mode::NONE;                   //Unknow line == error
                    break;
            }
        }

        Parse_line(line, scene);
        if(error_happen)
        {
            return  std::unexpected{error_message};
        }

        pos = end + 1;
    }

    return scene;
}

void eruptor::scene::Scene_parser::Load_file_to_buffor(const std::filesystem::path & scene_path)
{
    std::ifstream file{scene_path};
    if(!file)
    {
        error_happen = true;
        error_message = error_file_load;
        return;
    }

    std::ostringstream ss{};
    ss << file.rdbuf();

    buffor = ss.str();

    model_variables.clear();
    is_in_model_loading_stage = true;
    error_happen = false;
    error_message = "";
    line_mode = Line_mode::VERSION_CHECK;
    line_count = 0;
}

void eruptor::scene::Scene_parser::Parse_line(std::string_view line, Scene & scene)
{
    if(!line.empty() && line.back() == '\r')
    {
        line.remove_suffix(1);
    }

    switch(line_mode)
    {
        case Line_mode::VERSION_CHECK:
        {
            size_t colon_pos = line.find_first_of(":");

            if(colon_pos == std::string_view::npos || line.substr(0, colon_pos) != "Version")
            {
                std::println(std::cerr, "SCENE PARSER: Missing version declaration in top of the file: {}", line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            size_t version_start = line.find_first_not_of(" ", colon_pos + 1);
            if(version_start == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing version in version declaration: {}", line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            auto version = line.substr(version_start);
            if(version == "1_0")
            {
                file_version = Version::V1_0;
            }
            else if(version == "1_1")
            {
                file_version = Version::V1_1;
            }
            else if(version == "1_2")
            {
                file_version = Version::V1_2;
            }

            else
            {
                std::println(std::cerr, "SCENE PARSER: Unknown version in version declaration: {}", line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::MODEL_NAME:
        {
            if(line.back() != ']')
            {
                std::println(std::cerr, "SCENE PARSER: Missing closing model declaration bracket in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.find('=') == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing assigment sign in model declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            size_t last_model_name_pos = line.find_first_of(" =");
            size_t first_model_path_pos = line.find_first_not_of(" ", line.find('=') + 1);
            size_t last_model_path_pos = line.find_last_not_of(" ]", line.size());

            std::string_view model_name = line.substr(1, last_model_name_pos - 1);
            std::string_view model_path = line.substr( first_model_path_pos, last_model_path_pos - first_model_path_pos + 1);

            if(model_variables.find(model_name) != model_variables.end())
            {
                std::println(std::cerr, "SCENE PARSER: Redefinition of variable [[{}]] in line {}.", model_name, line_count);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            model_variables[model_name] = {model_path, resource_manager->Add_model(model_path)};
            current_parsed_model = &resource_manager->Get_model( model_variables[model_name].second );

            if constexpr(Debug_mode)
            {
                std::println(std::clog, "[{}] = [{}]", model_name, model_path);
            }

            if(file_version >= Version::V1_1)
            {
                line_mode = Line_mode::MODEL_HITBOX;
            }
            else
            {
                line_mode = Line_mode::NONE;
            }
            break;
        }
        case Line_mode::MODEL_HITBOX:
        {
            size_t colon_pos = line.find_first_of(":");

            if(line.substr(0, colon_pos) != "Hitbox" || colon_pos == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing hitbox declaration in model declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            size_t type_start = line.find_first_not_of(" ", colon_pos + 1);
            if(type_start == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing hitbox type in model hitbox declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            auto hitbox_type = line.substr(type_start);
            if(hitbox_type == "OBB")
            {
                current_parsed_model->hitbox_type = resource::Hitbox_type::OBB;
            }
            else if(hitbox_type == "SPHERE")
            {
                current_parsed_model->hitbox_type = resource::Hitbox_type::SPHERE;
            }
            else if(hitbox_type == "CAPSULE")
            {
                current_parsed_model->hitbox_type = resource::Hitbox_type::CAPSULE;
            }
            else
            {
                std::println(std::cerr, "SCENE PARSER: Unknown hitbox type in model hitbox declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::MODELS_DECLARATION_END:
        {
            if(line != "~~~~~")
            {
                std::println(std::cerr, "SCENE PARSER: Wrong amount of '~' sign in end of object declaration expresion in line {}: {}.", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            resource_manager->Load_models();

            is_in_model_loading_stage = false;
            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::OBJECT_DECLARATION:
        {
            if(is_in_model_loading_stage)
            {
                std::println(std::cerr, "SCENE PARSER: Declaring object before finish object loading stage in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.back() != '>')
            {
                std::println(std::cerr, "SCENE PARSER: Missing closing object declaration bracket in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            size_t last_object_name_pos = line.find_last_of('>');

            scene.render_objects.push_back({});
            scene.objects_aliases[ std::string{line.substr(1, last_object_name_pos - 1)} ] = scene.render_objects.size() - 1 ;
            scene.reverse_object_aliases[ scene.render_objects.size() - 1 ] = line.substr(1, last_object_name_pos - 1);

            line_mode = Line_mode::OBJECT_MODEL;
            break;
        }
        case Line_mode::OBJECT_MODEL:
        {
            size_t colon_pos = line.find_first_of(":");
            if(colon_pos == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing ':' in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.substr(0, colon_pos) != "Model")
            {
                std::println(std::cerr, "SCENE PARSER: Missing model for object declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            size_t first_variable_name_pos = line.find_first_not_of(" ", colon_pos + 1);
            std::string_view model_variable_name = line.substr(first_variable_name_pos);
            if(model_variables.find(model_variable_name) == model_variables.end())
            {
                std::println(std::cerr, "SCENE PARSER: Used model name not definded before used in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            scene.render_objects.back().Set_model(*resource_manager, model_variables[model_variable_name].second);

            line_mode = Line_mode::OBJECT_POSITION;
            break;
        }
        case Line_mode::OBJECT_POSITION:
        {
            size_t colon_pos = line.find_first_of(":");
            if(colon_pos == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing ':' in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.substr(0, colon_pos) != "Position")
            {
                std::println(std::cerr, "SCENE PARSER: Missing position for object declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            auto position = Parse_3_numbers( line.substr(colon_pos + 1) );
            if(position)
            {
                scene.render_objects.back().Set_position( position.value() );
            }
            else
            {
                error_happen = true;
                error_message = position.error();
                return;
            }

            line_mode = Line_mode::OBJECT_ROTATION;
            break;
        }
        case Line_mode::OBJECT_ROTATION:
        {
            size_t colon_pos = line.find_first_of(":");
            if(colon_pos == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing ':' in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.substr(0, colon_pos) != "Rotation")
            {
                std::println(std::cerr, "SCENE PARSER: Missing rotation for object declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            auto rotation_euler = Parse_3_numbers( line.substr(colon_pos + 1) );
            if(rotation_euler)
            {
                scene.render_objects.back().Set_rotation_euler( rotation_euler.value() );
            }
            else
            {
                error_happen = true;
                error_message = rotation_euler.error();
                return;
            }

            line_mode = Line_mode::OBJECT_SCALE;
            break;
        }
        case Line_mode::OBJECT_SCALE:
        {
            size_t colon_pos = line.find_first_of(":");
            if(colon_pos == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing ':' in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.substr(0, colon_pos) != "Scale")
            {
                std::println(std::cerr, "SCENE PARSER: Missing scale for object declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            auto scale = Parse_3_numbers( line.substr(colon_pos + 1) );
            if(scale)
            {
                scene.render_objects.back().Set_scale( *scale, scene.render_objects.back().Get_position().y );
            }
            else
            {
                error_happen = true;
                error_message = scale.error();
                return;
            }

            if(file_version >= Version::V1_2)
            {
                line_mode = Line_mode::OBJECT_COLOR;
            }
            else
            {
                line_mode = Line_mode::NONE;
            }
            break;
        }
        case Line_mode::OBJECT_COLOR:
        {
            size_t colon_pos = line.find_first_of(":");
            if(colon_pos == std::string_view::npos)
            {
                std::println(std::cerr, "SCENE PARSER: Missing ':' in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            if(line.substr(0, colon_pos) != "Color")
            {
                std::println(std::cerr, "SCENE PARSER: Missing color for object declaration in line {}: {}", line_count, line);

                error_happen = true;
                error_message = error_text_parsing;
                return;
            }

            auto color = Parse_3_numbers( line.substr(colon_pos + 1) );
            if(color)
            {
                scene.render_objects.back().color = resource::Color(color->x, color->y, color->z);
            }
            else
            {
                error_happen = true;
                error_message = color.error();
                return;
            }

            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::NONE:
            std::println(std::cerr, "SCENE PARSER: Unknown syntax in line {}: {}", line_count, line);

            error_happen = true;
            error_message = error_text_parsing;
            break;
    }
}

std::expected<glm::vec3, std::string_view> eruptor::scene::Scene_parser::Parse_3_numbers(std::string_view numbers)
{
    glm::vec3 result{};
    float * numbers_ptrs[3] {&result.x, &result.y, &result.z};

    size_t pos{};
    for(auto i{0UZ}; i < 3; i++)
    {
        pos = numbers.find_first_not_of(' ', pos);
        if(pos == std::string_view::npos)
        {
            std::println(std::cerr, "SCENE PARSER: Not enought numbers in line {}.", line_count);

            return std::unexpected{error_numbers_parsing};
        }

        size_t number_end = numbers.find_first_of(' ', pos);
        std::string_view number = (number_end == std::string_view::npos) ? numbers.substr(pos) : numbers.substr(pos, number_end - pos);

        auto [ptr, error_code] = std::from_chars(number.data(), number.data() + number.size(), *numbers_ptrs[i]);
        if(error_code != std::errc{} || ptr != number.data() + number.size())
        {
            std::println(std::cerr, "SCENE PARSER: Failed to parse numer '{}' in line {}.", number, line_count);

            return std::unexpected{error_numbers_parsing};
        }

        if(number_end == std::string_view::npos) break;
        pos = number_end;
    }

    return result;
}






