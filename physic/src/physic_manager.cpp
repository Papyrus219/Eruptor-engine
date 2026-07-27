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

void eruptor::physic::Physic_manager::On_event(const event::Event& event)
{

}
