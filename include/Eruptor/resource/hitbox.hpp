#ifndef ERUPTOR_RESOURCE_HITBOX_HPP
#define ERUPTOR_RESOURCE_HITBOX_HPP

#include <glm/glm.hpp>
#include <variant>

namespace eruptor::resource
{

struct OBB_hitbox
{
    glm::vec3 center{};
    glm::vec3 axies[3]{};
    glm::vec3 half_width{};
};

struct Sphere_hitbox
{
    glm::vec3 center{};
    float radius{};
};

using Hitbox = std::variant<OBB_hitbox, Sphere_hitbox>;

}

#endif //ERUPTOR_RESOURCE_HITBOX_HPP
