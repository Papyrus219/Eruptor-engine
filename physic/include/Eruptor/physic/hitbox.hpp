#ifndef ERUPTOR_PHYSIC_HITBOX_HPP
#define ERUPTOR_PHYSIC_HITBOX_HPP

#include <glm/glm.hpp>
#include <variant>

namespace eruptor::physic
{

struct AABB
{
    glm::vec3 min{};
    glm::vec3 max{};
};


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

Sphere_hitbox operator*(const Sphere_hitbox & sphere, const glm::mat4 & mat);
OBB_hitbox operator*(const OBB_hitbox & obb, const glm::mat4 & mat);

}

#endif //ERUPTOR_PHYSIC_HITBOX_HPP
