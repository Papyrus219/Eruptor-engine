#ifndef ERUPTOR_RESOURCE_RESOURCE_HPP
#define ERUPTOR_RESOURCE_RESOURCE_HPP

#include <filesystem>

namespace eruptor::resource
{

enum class Status
{
    UNINITIALIZED,
    PENDING,
    LODADED,
    ERROR
};

template<typename T>
struct Resource
{
    Status status{};
    std::filesystem::path path{};

    T resource{};
};

}

#endif //ERUPTOR_RESOURCE_RESOURCE_HPP
