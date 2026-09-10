#ifndef ERUPTOR_PHYSIC_COLISION_VISITOR_HPP
#define ERUPTOR_PHYSIC_COLISION_VISITOR_HPP

#include <glm/glm.hpp>
#include <Eruptor/physic/hitbox.hpp>

namespace eruptor::physic
{

class Colision_visitor
{
public:
    bool operator()(const Sphere_hitbox & a, const Sphere_hitbox & b) const
    {
        return Sphere_vs_sphere_test(a, b);
    }

    bool operator()(const Sphere_hitbox & sphere, const OBB_hitbox & obb) const
    {
        return Sphere_vs_OOB_test(sphere, obb);
    }

    bool operator()(const Sphere_hitbox & sphere, const Capsule_hitbox & capsule)
    {
        return Capsule_vs_Sphere_test(capsule, sphere);
    }

    bool operator()(const OBB_hitbox & a, const OBB_hitbox & b) const
    {
        return OBB_vs_OBB_test(a, b);
    }

    bool operator()(const OBB_hitbox & obb, const Sphere_hitbox & sphere) const
    {
        return Sphere_vs_OOB_test(sphere, obb);
    }

    bool operator()(const OBB_hitbox & obb, const Capsule_hitbox & capsule)
    {
        return Capsule_vs_OBB_test(capsule, obb);
    }

    bool operator()(const Capsule_hitbox & a, const Capsule_hitbox & b) const
    {
        return Capsule_vs_Capsule_test(a, b);
    }

    bool operator()(const Capsule_hitbox & capsule, const OBB_hitbox & obb)
    {
        return Capsule_vs_OBB_test(capsule, obb);
    }

    bool operator()(const Capsule_hitbox & capsule, const Sphere_hitbox & sphere)
    {
        return Capsule_vs_Sphere_test(capsule, sphere);
    }

    float delta_time{};

private:
    bool Sphere_vs_sphere_test(const Sphere_hitbox & a, const Sphere_hitbox & b) const;
    bool OBB_vs_OBB_test(const OBB_hitbox & a, const OBB_hitbox & b) const;
    bool Sphere_vs_OOB_test(const Sphere_hitbox & sphere, const OBB_hitbox & obb) const;
    bool Capsule_vs_Sphere_test(const Capsule_hitbox& cap, const Sphere_hitbox& sphere) const;
    bool Capsule_vs_Capsule_test(const Capsule_hitbox & c1, const Capsule_hitbox & c2) const;
    bool Capsule_vs_OBB_test(const Capsule_hitbox & cap, const OBB_hitbox & obb) const;

    glm::vec3 Closest_point_on_segment(const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & p) const
    {
        glm::vec3 ab = b - a;
        float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
        t = glm::clamp(t, 0.0f, 1.0f);
        return a + t * ab;
    }

};

}

#endif //ERUPTOR_PHYSIC_COLISION_VISITOR_HPP
