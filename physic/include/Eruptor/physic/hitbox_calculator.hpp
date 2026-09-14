#ifndef ERUPTOR_PHYSIC_HITBOX_CALCULATOR_HPP
#define ERUPTOR_PHYSIC_HITBOX_CALCULATOR_HPP

#include <Eruptor/physic/hitbox.hpp>
#include <vector>

namespace eruptor::physic
{

class Hitbox_calculator
{
public:
    void Calculate_sphere_hitbox(physic::Sphere_hitbox & sphere, std::vector<glm::vec3> & all_vertecies);
    void Calculate_obb_hitbox(physic::OBB_hitbox & obb, std::vector<glm::vec3> & all_vertecies);
    void Calculate_capsule_hitbox(physic::Capsule_hitbox & capsule, std::vector<glm::vec3> & all_vertecies);

private:
    glm::mat3 Compute_covariance(const std::vector<glm::vec3> & all_vertecies, glm::vec3 & centroid);
    glm::mat3 Jacobi_eigenvectors(glm::mat3 & cov, size_t iterations = 20);
};

}

#endif // ERUPTOR_PHYSIC_HITBOX_CALCULATOR_HPP
