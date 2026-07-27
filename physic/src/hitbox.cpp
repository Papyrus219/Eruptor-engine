#include <Eruptor/physic/hitbox.hpp>
#include <algorithm>

eruptor::physic::Sphere_hitbox eruptor::physic::operator*(const Sphere_hitbox & sphere, const glm::mat4 & mat)
{
    Sphere_hitbox result{};
    result.center = glm::vec3(mat * glm::vec4(sphere.center, 1.0f));

    float scale_x = glm::length(glm::vec3{mat[0]});
    float scale_y = glm::length(glm::vec3{mat[1]});
    float scale_z = glm::length(glm::vec3{mat[2]});
    float max_scale = std::max({scale_x, scale_y, scale_z});

    result.radius = sphere.radius * max_scale;
    return result;
}

eruptor::physic::OBB_hitbox eruptor::physic::operator*(const OBB_hitbox & obb, const glm::mat4& mat)
{
    OBB_hitbox result{};
    result.center = glm::vec3{mat * glm::vec4{obb.center, 1.0f}};

    float scale_x = glm::length(glm::vec3{mat[0]});
    float scale_y = glm::length(glm::vec3{mat[1]});
    float scale_z = glm::length(glm::vec3{mat[2]});

    glm::mat3 rot_mat{ glm::vec3{mat[0]} / scale_x, glm::vec3{mat[1]} / scale_y, glm::vec3{mat[2]} / scale_z};

    for(auto i{0UZ}; i < 3; i++)
    {
        result.axies[i] = glm::normalize(rot_mat * obb.axies[i]);
    }

    result.half_width.x = obb.half_width.x * scale_x;
    result.half_width.y = obb.half_width.y * scale_y;
    result.half_width.z = obb.half_width.z * scale_z;

    return result;
}
