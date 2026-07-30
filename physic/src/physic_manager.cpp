#include <Eruptor/physic/physic_manager.hpp>
#include <Eruptor/scene/scene.hpp>
#include <Eruptor/event/event_manager.hpp>

eruptor::physic::Physic_manager::Physic_manager(): event_manager{ event::event_manager }
{
    event_manager.Add_listener( *this );
}

void eruptor::physic::Physic_manager::Chceck_colisions(scene::Scene & scene)
{
    for(auto & object : scene.render_objects)
    {
        object.is_coliding = false;
    }

    can_coliding.clear();

    for(auto i{1UZ}; i < scene.render_objects.size(); i++)
    {
        for(auto j{i + 1}; j < scene.render_objects.size(); j++)
        {
            auto aabb_a = scene.render_objects[i].Get_aabb();
            auto aabb_b = scene.render_objects[j].Get_aabb();

            bool x_colision = aabb_a.max.x > aabb_b.min.x && aabb_a.min.x < aabb_b.max.x;
            bool y_colision = aabb_a.max.y > aabb_b.min.y && aabb_a.min.y < aabb_b.max.y;
            bool z_colision = aabb_a.max.z > aabb_b.min.z && aabb_a.min.z < aabb_b.max.z;

            if(x_colision && y_colision && z_colision)
            {
                can_coliding.push_back( {i, j} );
            }
        }
    }

    for(auto i{0UZ}; i < can_coliding.size(); i++)
    {
        if( std::visit(colision_visitor, scene.render_objects[ can_coliding[i].first ].Get_hitbox(), scene.render_objects[ can_coliding[i].second ].Get_hitbox()) )
        {
            scene.render_objects[ can_coliding[i].first ].is_coliding = true;
            scene.render_objects[ can_coliding[i].second ].is_coliding = true;

            event::Event::Collision_occurred colision{};
            colision.object_a_id = can_coliding[i].first;
            colision.object_b_id = can_coliding[i].second;

            event_manager.Announce_event( colision );
        }
    }
}

bool eruptor::physic::Physic_manager::Colision_visitor::Sphere_vs_sphere_test(const Sphere_hitbox& a, const Sphere_hitbox& b) const
{
    float dist_squared = glm::dot(b.center - a.center, b.center - a.center);
    float radius_sum = a.radius + b.radius;

    return dist_squared <= (radius_sum * radius_sum);
}

bool eruptor::physic::Physic_manager::Colision_visitor::OBB_vs_OBB_test(const OBB_hitbox& a, const OBB_hitbox& b) const
{
    glm::vec3 axes_to_test[16]{};
    int id_x{};

    for(auto i{0UZ}; i < 3; i++)
    {
        axes_to_test[id_x++] = a.axies[i];
    }

    for(auto i{0UZ}; i < 3; i++)
    {
        axes_to_test[id_x++] = b.axies[i];
    }

    for(auto i{0UZ}; i < 3; i++)
    {
        for(auto j{0UZ}; j < 3; j++)
        {
            axes_to_test[id_x++] = glm::cross(a.axies[i], b.axies[j]);
        }
    }

    glm::vec3 t = b.center - a.center;

    for(const auto & axis : axes_to_test)
    {
        if(glm::dot(axis, axis) < 1e-6f) continue;

        glm::vec3 norm_axis = glm::normalize(axis);

        float proj_a{}, proj_b{};
        for(auto i{0UZ}; i < 3; i++)
        {
            proj_a += a.half_width[i] * std::abs(glm::dot(a.axies[i], norm_axis));
            proj_b += b.half_width[i] * std::abs(glm::dot(b.axies[i], norm_axis));
        }

        float distance = std::abs(glm::dot(t, norm_axis));

        if(distance > (proj_a + proj_b + 1e-4f))
        {
            return false;
        }
    }

    return true;
}

bool eruptor::physic::Physic_manager::Colision_visitor::Sphere_vs_OOB_test(const Sphere_hitbox& sphere, const OBB_hitbox& obb) const
{
    glm::vec3 dis = sphere.center - obb.center;
    glm::vec3 closest_point = obb.center;

    for(auto i{0UZ}; i < 3; i++)
    {
        float distance = glm::dot(dis, obb.axies[i]);
        distance = glm::clamp(distance, -obb.half_width[i], obb.half_width[i]);
        closest_point += distance * obb.axies[i];
    }

    glm::vec3 diff = sphere.center - closest_point;

    return glm::dot(diff, diff) <= sphere.radius * sphere.radius;
}

bool eruptor::physic::Physic_manager::Colision_visitor::Capsule_vs_Sphere_test(const Capsule_hitbox & cap, const Sphere_hitbox & sphere) const
{
    glm::vec3 closest = Closest_point_on_segment(cap.start, cap.end, sphere.center);
    float dist_sq = glm::dot(sphere.center - closest, sphere.center - closest);
    float radius_sum = cap.radius + sphere.radius;

    return dist_sq <= (radius_sum * radius_sum);
}

bool eruptor::physic::Physic_manager::Colision_visitor::Capsule_vs_Capsule_test(const Capsule_hitbox & cap_1, const Capsule_hitbox & cap_2) const
{
    glm::vec3 d1 = cap_1.end - cap_1.start;
    glm::vec3 d2 = cap_2.end - cap_2.start;
    glm::vec3 r = cap_1.start - cap_2.start;

    float a = glm::dot(d1, d1);
    float e = glm::dot(d2, d2);
    float f = glm::dot(d2, r);

    float s = 0.0f, t = 0.0f;

    if (a <= 1e-6f && e <= 1e-6f)
    {
        return glm::dot(cap_1.start - cap_2.start, cap_1.start - cap_2.start) <= (cap_1.radius + cap_2.radius) * (cap_1.radius + cap_2.radius);
    }

    if (a <= 1e-6f)
    {
        s = 0.0f;
        t = glm::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = glm::dot(d1, r);
        if (e <= 1e-6f)
        {
            t = 0.0f;
            s = glm::clamp(-c / a, 0.0f, 1.0f);
        }
        else
        {
            float b = glm::dot(d1, d2);
            float denom = a * e - b * b;

            if (denom != 0.0f)
            {
                s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0.0f;
            }

            t = (b * s + f) / e;
            if(t < 0.0f)
            {
                t = 0.0f;
                s = glm::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = glm::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    glm::vec3 p1 = cap_1.start + d1 * s;
    glm::vec3 p2 = cap_2.start + d2 * t;
    float dist_sq = glm::dot(p1 - p2, p1 - p2);
    float radius_sum = cap_1.radius + cap_2.radius;

    return dist_sq <= (radius_sum * radius_sum);
}

bool eruptor::physic::Physic_manager::Colision_visitor::Capsule_vs_OBB_test(const Capsule_hitbox & cap, const OBB_hitbox & obb) const
{
    auto transform_to_local = [&obb](const glm::vec3& p)
    {
        glm::vec3 d = p - obb.center;
        return glm::vec3{ glm::dot(d, obb.axies[0]), glm::dot(d, obb.axies[1]), glm::dot(d, obb.axies[2]) };
    };

    glm::vec3 local_a = transform_to_local(cap.start);
    glm::vec3 local_b = transform_to_local(cap.end);

    glm::vec3 ba = local_b - local_a;
    float ba_len_sq = glm::dot(ba, ba);

    int steps = 10;
    float min_dist_sq = std::numeric_limits<float>::max();

    for(int i = 0; i <= steps; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(steps);
        glm::vec3 p_local = local_a + ba * t;

        glm::vec3 closest_on_aabb = glm::clamp(p_local, -obb.half_width, obb.half_width);

        float dist_sq = glm::dot(p_local - closest_on_aabb, p_local - closest_on_aabb);
        if(dist_sq < min_dist_sq)
        {
            min_dist_sq = dist_sq;
        }
    }

    return min_dist_sq <= (cap.radius * cap.radius);
}

void eruptor::physic::Physic_manager::On_event(const event::Event& event)
{

}
