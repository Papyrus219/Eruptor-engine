#include <Eruptor/physic/physic_manager.hpp>
#include <Eruptor/scene/scene.hpp>
#include <Eruptor/event/event_manager.hpp>

eruptor::physic::Physic_manager::Physic_manager(): event_manager{ event::event_manager }
{
    event_manager.Add_listener( *this );
}

void eruptor::physic::Physic_manager::Chceck_aabbs_colisions(scene::Scene & scene)
{
    for(auto & object : scene.render_objects)
    {
        object.is_coliding = false;
    }

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
                scene.render_objects[i].is_coliding = true;
                scene.render_objects[j].is_coliding = true;

                event::Event::Collision_occurred colision{};
                colision.object_a_id = i;
                colision.object_b_id = j;

                event_manager.Announce_event( colision );
            }
        }
    }
}

void eruptor::physic::Physic_manager::On_event(const event::Event& event)
{

}
