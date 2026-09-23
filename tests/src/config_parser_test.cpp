#include <Eruptor/resource/config_parser.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Parsing configuration file" )
{
    eruptor::resource::Config_parser parser{};
    auto result_wraper = parser.Parse_config_file("../../tests/data/conf_test.cfg");

    if(result_wraper)
    {
        auto & result = result_wraper.value();

        REQUIRE(result.at("Var 1") == "Lolalalsa");
        REQUIRE(result.at("Var 2") == "34.32");
        REQUIRE(result.at("Formula") == "(sense * sense * size) + speed");
    }
}

TEST_CASE( "Convert parsed text to number" )
{
    eruptor::resource::Config_parser parser{};
    auto result_wraper = parser.Parse_config_file("../../tests/data/conf_test.cfg");

    if(result_wraper)
    {
        auto & result = result_wraper.value();

        REQUIRE(result.at("Var 2") == "34.32");

        auto & number_str = result.at("Var 2");
        float number{};
        parser.Convert_string_to_number(number_str, number);

        REQUIRE(number == 34.32);
    }
}
