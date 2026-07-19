#ifndef ERUPTOR_HARDWARE_MEMORY_UNITS_HPP
#define ERUPTOR_HARDWARE_MEMORY_UNITS_HPP

#include <vulkan/vulkan.hpp>

namespace eruptor::hardware
{

inline namespace units
{

consteval vk::DeviceSize operator""_KiB(unsigned long long size)
{
    return static_cast<vk::DeviceSize>( size * 1024 );
}

consteval vk::DeviceSize operator""_MiB(unsigned long long size)
{
    return static_cast<vk::DeviceSize>( size * 1024_KiB );
}

consteval vk::DeviceSize operator""_GiB(unsigned long long size)
{
    return static_cast<vk::DeviceSize>( size * 1024_MiB );
}

consteval vk::DeviceSize operator""_KiB(long double size)
{
    return static_cast<vk::DeviceSize>( size * 1024 );
}

consteval vk::DeviceSize operator""_MiB(long double size)
{
    return static_cast<vk::DeviceSize>( size * 1024_KiB );
}

consteval vk::DeviceSize operator""_GiB(long double size)
{
    return static_cast<vk::DeviceSize>( size * 1024_MiB );
}

}

}

#endif //ERUPTOR_HARDWARE_MEMORY_UNITS_HPP
