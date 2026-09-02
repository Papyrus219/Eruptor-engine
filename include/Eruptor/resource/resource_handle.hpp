#ifndef ERUPTOR_RESOURCE_RESOURCE_HANDLE_HPP
#define ERUPTOR_RESOURCE_RESOURCE_HANDLE_HPP

#include <cstdint>

namespace eruptor::resource
{

struct Model_tag{};
struct Mesh_tag{};
struct Hardware_Texture_tag{};
struct Resource_Texture_tag{};
struct Material_tag{};
struct Font_tag{};

template<typename TAG>
class Resource_handle
{
public:
    constexpr explicit Resource_handle() = default;
    constexpr explicit Resource_handle(const uint32_t & id): resource_id{id} {}

    bool Is_valid() const {return resource_id != 0;}
    const uint32_t & Get_id() const {return resource_id;}

    bool operator==(const Resource_handle & other) const {return this->resource_id == other.resource_id;}
    bool operator!=(const Resource_handle & other) const {return this->resource_id != other.resource_id;}

    operator bool() const {return resource_id != 0;}
    operator uint32_t() const {return resource_id;}

private:
    uint32_t resource_id{};
};

using Model_handle = Resource_handle<Model_tag>;
using Mesh_handle = Resource_handle<Mesh_tag>;
using Hardware_Texture_handle = Resource_handle<Hardware_Texture_tag>;
using Resource_Texture_handle = Resource_handle<Resource_Texture_tag>;
using Material_handle = Resource_handle<Material_tag>;
using Font_handle = Resource_handle<Font_tag>;

}

#endif // ERUPTOR_RESOURCE_RESOURCE_HANDLE_HPP
