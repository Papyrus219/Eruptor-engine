#ifndef ERUPTOR_HARDWARE_GEOMETRY_BUFFER_HPP
#define ERUPTOR_HARDWARE_GEOMETRY_BUFFER_HPP

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_raii.hpp>

namespace eruptor::hardware
{

struct Geometry_buffer
{
    vma::raii::Buffer vertex_buffer = nullptr;
    vma::raii::Buffer index_buffer = nullptr;
};

}

#endif // ERUPTOR_HARDWARE_GEOMETRY_BUFFER_HPP
