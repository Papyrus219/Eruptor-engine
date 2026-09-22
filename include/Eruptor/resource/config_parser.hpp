#ifndef OVUM_CONFIG_PARSER_HPP
#define OVUM_CONFIG_PARSER_HPP

#include <unordered_map>
#include <filesystem>
#include <expected>
#include <string>
#include <cstdint>

namespace eruptor::resource
{

class Config_parser
{
public:
    std::expected<std::unordered_map<std::string, std::string>, std::string_view> Parse_config_file(const std::filesystem::path & path);

private:
    std::string buffor{};
    std::string error_message{};
    uint32_t line_count{};
};

}

#endif // OVUM_CONFIG_PARSER_HPP
