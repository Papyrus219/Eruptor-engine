#include <Eruptor/scene/scene_parser.hpp>
#include <fstream>
#include <iostream>
#include <print>

#ifndef NDEBUG
constexpr bool Debug_mode = true;
#else
constexpr bool Debug_mode = false;
#endif

std::string eruptor::scene::Scene_parser::error_file_load{"Failed to load file!"};
std::string eruptor::scene::Scene_parser::error_parsing{"Failed to parse file correctly!"};

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
        if(line.size() == 0) //If line empty skip.
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
                    line_mode = Line_mode::MODEL_NAME;                  //Model path declaration
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
        if(!is_parsing)
        {
            error_happen = true;
            error_message = error_parsing;
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
    is_parsing = true;
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
                std::print(std::cerr, "Missing version declaration in top of the file: {}\n", line);
                is_parsing = false;
                return;
            }

            size_t version_start = line.find_first_not_of(" ", colon_pos + 1);
            if(version_start == std::string_view::npos)
            {
                std::print(std::cerr, "Missing version in version declaration: {}\n", line);
                is_parsing = false;
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
            else
            {
                std::print(std::cerr, "Unknown version in version declaration: {}\n", line);
                is_parsing = false;
                return;
            }

            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::MODEL_NAME:
        {
            if(line.back() != ']')
            {
                std::print(std::cerr, "Missing closing model declaration bracket in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            if(line.find('=') == std::string_view::npos)
            {
                std::print(std::cerr, "Missing assigment sign in model declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            size_t last_model_name_pos = line.find_first_of(" =");
            size_t first_model_path_pos = line.find_first_not_of(" ", line.find('=') + 1);
            size_t last_model_path_pos = line.find_last_not_of(" ]", line.size());

            std::string_view model_name = line.substr(1, last_model_name_pos - 1);
            std::string_view model_path = line.substr( first_model_path_pos, last_model_path_pos - first_model_path_pos + 1);

            if(model_variables.find(model_name) != model_variables.end())
            {
                std::print(std::cerr, "Redefinition of variable [[{}]] in line {}.\n", model_name, line_count);
                is_parsing = false;
                return;
            }

            model_variables[model_name] = {model_path, resource_manager->Add_model(model_path)};
            current_parsed_model = &resource_manager->Get_model( model_variables[model_name].second );

            if constexpr(Debug_mode)
            {
                std::print(std::clog, "[{}] = [{}]\n", model_name, model_path);
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
                std::print(std::cerr, "Missing hitbox declaration in model declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            size_t type_start = line.find_first_not_of(" ", colon_pos + 1);
            if(type_start == std::string_view::npos)
            {
                std::print(std::cerr, "Missing hitbox type in model hitbox declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
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
            else
            {
                std::print(std::cerr, "Unknown hitbox type in model hitbox declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::MODELS_DECLARATION_END:
        {
            if(line != "~~~~~")
            {
                std::print(std::cerr, "Wrong amount of '~' sign in end of object declaration expresion in line {}: {}.\n", line_count, line);
                is_parsing = false;
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
                std::print(std::cerr, "Declaring object before finish object loading stage in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            if(line.back() != '>')
            {
                std::print(std::cerr, "Missing closing object declaration bracket in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            size_t last_object_name_pos = line.find_last_of('>');

            scene.render_objects.push_back({});
            scene.objects_aliases[ std::string{line.substr(1, last_object_name_pos - 1)} ] = scene.render_objects.size() - 1 ;

            line_mode = Line_mode::OBJECT_MODEL;
            break;
        }
        case Line_mode::OBJECT_MODEL:
        {
            if(line.substr(0, line.find_first_of(":")) != "Model")
            {
                std::print(std::cerr, "Missing model for object declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            size_t first_variable_name_pos = line.find_first_not_of(" ", line.find_first_of(":") + 1);
            std::string_view model_variable_name = line.substr(first_variable_name_pos);
            if(model_variables.find(model_variable_name) == model_variables.end())
            {
                std::print(std::cerr, "Used model name not definded before used in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            scene.render_objects.back().Set_model(*resource_manager, model_variables[model_variable_name].second);

            line_mode = Line_mode::OBJECT_POSITION;
            break;
        }
        case Line_mode::OBJECT_POSITION:
        {
            if(line.substr(0, line.find_first_of(":")) != "Position")
            {
                std::print(std::cerr, "Missing position for object declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            scene.render_objects.back().Set_position( Parse_3_numbers( line.substr(line.find_first_of(":") + 1) ) );

            line_mode = Line_mode::OBJECT_ROTATION;
            break;
        }
        case Line_mode::OBJECT_ROTATION:
        {
            if(line.substr(0, line.find_first_of(":")) != "Rotation")
            {
                std::print(std::cerr, "Missing rotation for object declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            scene.render_objects.back().Set_rotation_euler( Parse_3_numbers( line.substr(line.find_first_of(":") + 1) ) );

            line_mode = Line_mode::OBJECT_SCALE;
            break;
        }
        case Line_mode::OBJECT_SCALE:
        {
            if(line.substr(0, line.find_first_of(":")) != "Scale")
            {
                std::print(std::cerr, "Missing scale for object declaration in line {}: {}\n", line_count, line);
                is_parsing = false;
                return;
            }

            scene.render_objects.back().Set_scale( Parse_3_numbers( line.substr(line.find_first_of(":") + 1) ), scene.render_objects.back().Get_position().y );

            line_mode = Line_mode::NONE;
            break;
        }
        case Line_mode::NONE:
            std::print(std::cerr, "Unknown syntax in line {}: {}\n", line_count, line);
            is_parsing = false;
            break;
    }
}

glm::vec3 eruptor::scene::Scene_parser::Parse_3_numbers(std::string_view numbers)
{
    glm::vec3 result{};

    size_t next_pos = numbers.find_first_not_of(' ');

    std::from_chars(numbers.data() + next_pos, numbers.data() + (numbers.find_first_of(" ", next_pos)), result.x );
    next_pos = numbers.find_first_of(" ", next_pos) + 1;
    std::from_chars(numbers.data() + next_pos, numbers.data() + (numbers.find_first_of(" ", next_pos)), result.y);
    next_pos = numbers.find_first_of(" ", next_pos) + 1;
    std::from_chars(numbers.data() + next_pos, numbers.data() + numbers.size(), result.z);

    return result;
}








