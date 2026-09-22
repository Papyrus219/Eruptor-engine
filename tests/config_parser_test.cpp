#include <iostream>
#include <Eruptor/resource/config_parser.hpp>

int main()
{
    eruptor::resource::Config_parser parser{};

    auto result = parser.Parse_config_file("../../tests/conf_test.cfg");

    if(result)
    {
        for(auto & [name, val] : result.value())
        {
            std::cout << name << " -> " << val << '\n';
        }
    }
    else
    {
        std::cerr << result.error() << '\n';
    }

    return 0;
}
