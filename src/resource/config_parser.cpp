#include <Eruptor/resource/config_parser.hpp>
#include <fstream>
#include <format>

using namespace eruptor::resource;

std::expected<std::unordered_map<std::string, std::string>, std::string_view> eruptor::resource::Config_parser::Parse_config_file(const std::filesystem::path & path)
{
    std::unordered_map<std::string, std::string> parsed_config{};
    std::ifstream file{path};
    if(!file)
    {
        error_message = "CONFIG_PARSER::Failed to open a file.";
        return std::unexpected{ std::string_view{error_message} };
    }

    std::ostringstream ss{};
    ss << file.rdbuf();
    buffor = ss.str();
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

        if(line[0] == '#')
        {
            pos = end + 1;
            continue;
        }

        size_t first_tilde_pos = line.find_first_of('~');
        size_t secound_tilde_pos = line.find_first_of('~', first_tilde_pos + 1);
        if(first_tilde_pos == std::string_view::npos)
        {
            error_message = std::format("CONFIG_PARSER::No tilde in varaible name definition!\n Line {}: {}", line_count, line);
            return std::unexpected{ std::string_view{error_message} };
        }
        if(secound_tilde_pos == std::string_view::npos)
        {
            error_message = std::format("CONFIG_PARSER::No secound tilde in varaible name definition!\n Line {}: {}", line_count, line);
            return std::unexpected{ std::string_view{error_message} };
        }

        size_t varible_value_begin = line.find_first_not_of(' ', secound_tilde_pos + 1);

        if(varible_value_begin == std::string_view::npos)
        {
            error_message = std::format("CONFIG_PARSER::No varaible value!\n Line {}: {}", line_count, line);
            return std::unexpected{ std::string_view{error_message} };
        }

        size_t hash_pos = line.find_first_of('#');
        size_t line_end = (hash_pos == std::string_view::npos)? line.size() : hash_pos;

        size_t variable_value_end = line_end;
        while(variable_value_end > varible_value_begin && (line[variable_value_end - 1] == ' ' || line[variable_value_end - 1] == '\t' || line[variable_value_end - 1] == '\r'))
        {
            variable_value_end--;
        }

        std::string_view variable_name = line.substr(first_tilde_pos + 1, secound_tilde_pos - first_tilde_pos - 1);
        std::string_view variable_value = line.substr(varible_value_begin, variable_value_end - varible_value_begin);

        parsed_config[ std::string(variable_name) ] = std::string(variable_value);
        pos = end + 1;
    }

    return std::move(parsed_config);
}
