#ifndef ERUPTOR_PHYSIC_COLISION_BOX_HPP
#define ERUPTOR_PHYSIC_COLISION_BOX_HPP

#include <glm/glm.hpp>

namespace eruptor::physic
{

struct AABB
{
    glm::vec3 min{};
    glm::vec3 max{};
};

struct OOB
{
    glm::vec3 center{};
    glm::vec3 half_size{};
    glm::mat3 rotation{1.0f};
};

struct Colision_box
{

};

}

#endif // ERUPTOR_PHYSIC_COLISION_BOX_HPP
