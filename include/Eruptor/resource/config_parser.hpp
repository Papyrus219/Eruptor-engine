#ifndef ERUPOR_RESOURCE_CONFIG_PARSER_HPP
#define ERUPOR_RESOURCE_CONFIG_PARSER_HPP

#include <unordered_map>
#include <filesystem>
#include <expected>
#include <string>
#include <cstdint>
#include <iostream>

namespace eruptor::resource
{

class Config_parser
{
public:
    std::expected<std::unordered_map<std::string, std::string>, std::string_view> Parse_config_file(const std::filesystem::path & path);

    template<typename T>
    void Convert_string_to_number(std::string_view str, T & var, std::string_view var_name = "NOT GIVEN");

private:
    std::string buffor{};
    std::string error_message{};
    uint32_t line_count{};
};

template<typename T>
void eruptor::resource::Config_parser::Convert_string_to_number(std::string_view str, T & var, std::string_view var_name)
{
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), var);
    if(ec != std::errc{})
    {
        std::println(std::cerr, "Failed to parse variable: {}", var_name);
    }
}

}

#endif // ERUPOR_RESOURCE_CONFIG_PARSER_HPP
