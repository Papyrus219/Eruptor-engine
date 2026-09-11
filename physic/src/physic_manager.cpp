#include <Eruptor/physic_manager.hpp>
#include <Eruptor/scene/scene.hpp>
#include <Eruptor/event/event_manager.hpp>
#include <Eruptor/resource_manager.hpp>

eruptor::physic::Physic_manager::Physic_manager(): event_manager{ event::event_manager }
{
    event_manager.Add_listener( *this );
}

void eruptor::physic::Physic_manager::Update_scene(scene::Scene & scene, float delta_time)
{
    Snap_y(scene);

    Chceck_colisions(scene, delta_time);
}

void eruptor::physic::Physic_manager::Add_hitbox(uint8_t layer, uint32_t render_id, scene::Scene & scene)
{
    ///@todo Finish adding hitbox api
    hitboxes_data[layer].emplace_back();
    hitboxes_data[layer].back().Set_render_object_id( render_id );
    hitboxes_data[layer].back().Set_model( *this, scene.render_objects[ render_id ].Get_model_handle().Get_id() );

}

void eruptor::physic::Physic_manager::Add_model_hitbox(uint32_t model_resource_id, physic::Hitbox_type hitbox_type, std::vector<glm::vec3>& all_vertecies)
{
    physic::AABB aabb{glm::vec3{std::numeric_limits<float>::max()}, glm::vec3{std::numeric_limits<float>::lowest()}};
    physic::Hitbox hitbox{};

    for(const auto & vert : all_vertecies)
    {
        aabb.min = glm::min(vert, aabb.min);
        aabb.max = glm::max(vert, aabb.max);
    }

    if(hitbox_type == physic::Hitbox_type::OBB)
    {
        physic::OBB_hitbox oob_hitbox{};

        hitbox_calculator.Calculate_obb_hitbox(oob_hitbox, all_vertecies);

        hitbox = oob_hitbox;
    }
    else if(hitbox_type == physic::Hitbox_type::SPHERE)
    {
        physic::Sphere_hitbox sphere_hitbox{};

        hitbox_calculator.Calculate_sphere_hitbox(sphere_hitbox, all_vertecies);

        hitbox = sphere_hitbox;
    }
    else if(hitbox_type ==  physic::Hitbox_type::CAPSULE)
    {
        physic::Capsule_hitbox capsule_hitbox{};

        hitbox_calculator.Calculate_capsule_hitbox(capsule_hitbox, all_vertecies);

        hitbox = capsule_hitbox;
    }

    model_aabbs[model_resource_id] = aabb;
    model_hitboxes[model_resource_id] = hitbox;
}

void eruptor::physic::Physic_manager::Snap_y(scene::Scene & scene)
{
    for(auto & hitbox_data : hitboxes_data[0])
    {
        auto & render_object = scene.render_objects[ hitbox_data.Get_render_object_id() ];
        if(render_object.snap_y)
        {
            physic::AABB aabb = hitbox_data.Get_aabb(scene);
            glm::vec3 pos = render_object.Get_position();
            pos.y += render_object.snap_y.value() - aabb.min.y;
            render_object.Set_position( pos );

            render_object.aabb_has_changed = true;
            render_object.hitbox_has_changed = true;
        }
    }
}

void eruptor::physic::Physic_manager::Chceck_colisions(scene::Scene & scene, float delta_time)
{
    can_coliding.clear();

    for(auto i{1UZ}; i < hitboxes_data[0].size(); i++)
    {
        for(auto j{i + 1}; j < hitboxes_data[0].size(); j++)
        {
            auto aabb_a = hitboxes_data[0][i].Get_swept_aabb(scene);
            auto aabb_b = hitboxes_data[0][j].Get_swept_aabb(scene);

            bool x_colision = aabb_a.max.x > aabb_b.min.x && aabb_a.min.x < aabb_b.max.x;
            bool y_colision = aabb_a.max.y > aabb_b.min.y && aabb_a.min.y < aabb_b.max.y;
            bool z_colision = aabb_a.max.z > aabb_b.min.z && aabb_a.min.z < aabb_b.max.z;

            if(x_colision && y_colision && z_colision)
            {
                can_coliding.push_back( {i, j} );
            }
        }
    }

    colision_visitor.delta_time = delta_time;

    for(auto i{0UZ}; i < can_coliding.size(); i++)
    {
        if( std::visit(colision_visitor, hitboxes_data[0][ can_coliding[i].first ].Get_hitbox(scene), hitboxes_data[0][ can_coliding[i].second ].Get_hitbox(scene)) )
        {
            event::Event::Collision_occurred colision{};
            colision.object_a_id = can_coliding[i].first;
            colision.object_b_id = can_coliding[i].second;

            event_manager.Announce_event( colision );
        }
    }
}

void eruptor::physic::Physic_manager::On_event(const event::Event & event)
{
    if(auto model_change = event.Get_if<event::Event::Render_object_changed_model>())
    {
        for(auto hitbox_data : hitboxes_data[0])
        {
            if(hitbox_data.Get_render_object_id() == model_change->render_object_id)
            {
                hitbox_data.Set_model(*this, model_change->render_object_id);
            }
        }
    }
}
