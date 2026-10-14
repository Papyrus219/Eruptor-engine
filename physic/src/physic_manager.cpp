#include <Eruptor/physic_manager.hpp>
#include <Eruptor/scene/scene.hpp>
#include <Eruptor/event/event_manager.hpp>
#include <Eruptor/resource_manager.hpp>
#include <ranges>
#include <algorithm>
#include <limits>
#include <bit>
#include <cassert>
#include <cmath>
#include <algorithm>

eruptor::physic::Physic_manager::Physic_manager(): event_manager{ event::event_manager }
{
    event_manager.Add_listener( *this );
    layer_colision_masks.resize( LAYERS_AMOUNT );
}

void eruptor::physic::Physic_manager::Update_scene(scene::Scene & scene, float delta_time)
{
    Snap_y(scene);

    Fill_grid(scene);
    Chceck_colisions(scene, delta_time);
}

void eruptor::physic::Physic_manager::Setup_grid(uint32_t rows_amount, uint32_t columns_amount, AABB scene_aabb, uint32_t average_amount_for_field)
{
    grid.Init(rows_amount, columns_amount, scene_aabb.min, scene_aabb.max, average_amount_for_field);
}

std::optional<std::reference_wrapper<eruptor::physic::Hitbox_metadata>> eruptor::physic::Physic_manager::Get_hitbox_data(uint8_t layer, uint32_t render_id)
{
    auto result = std::find_if(hitboxes_data[layer].begin(), hitboxes_data[layer].end(), [render_id](auto & hitbox_data)
                                                                            {return hitbox_data.Get_render_object_id() == render_id;}) ;

    if(result == hitboxes_data[layer].end())
    {
        return {};
    }

    return *result;
}

void eruptor::physic::Physic_manager::Add_hitbox(uint8_t layer, uint32_t render_id, scene::Scene & scene)
{
    auto result = std::find_if(hitboxes_data[layer].begin(), hitboxes_data[layer].end(), [render_id](auto & hitbox_data)
    {return hitbox_data.Get_render_object_id() == render_id;}) ;

    if(result == hitboxes_data[layer].end())
    {
        hitboxes_data[layer].emplace_back();
        hitboxes_data[layer].back().Set_render_object_id( render_id );
        hitboxes_data[layer].back().Set_model( *this, scene.render_objects[ render_id ].Get_model_handle().Get_id() );
    }
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
    model_hitboxes_types[model_resource_id] = hitbox_type;
}

void eruptor::physic::Physic_manager::Request_layers_colision_test(uint8_t requesting_layer, uint8_t requested_layer)
{
    assert(requesting_layer < LAYERS_AMOUNT && requested_layer < LAYERS_AMOUNT);

    auto mask = 1u << requested_layer;

    layer_colision_masks[requesting_layer] |= mask;
    requested_layers |= mask;
}

void eruptor::physic::Physic_manager::Fill_grid(scene::Scene & scene)
{
    grid.Clear();

    for(uint32_t m = requested_layers; m != 0; m &= m-1)
    {
        auto layer_id = std::countr_zero( m );
        auto & layer = hitboxes_data[ layer_id ];

        for(auto i{0UZ}; i < layer.size(); i++)
        {
            if(!layer[i].Get_is_active()) continue;

            grid.Add_object_to_grid( layer[i].Get_swept_aabb(scene), i, layer_id);
        }
    }
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

            render_object.transformation_version++;
        }
    }
}

#include <iostream>

void eruptor::physic::Physic_manager::Chceck_colisions(scene::Scene & scene, float delta_time)
{
    can_coliding.clear();
    can_coliding.reserve( last_can_coliding_size );

    for(uint8_t i{0}; i < layer_colision_masks.size(); i++)
    {
        auto mask = layer_colision_masks[i];
        if(mask == 0) continue;

        for(uint32_t j{0}; j < hitboxes_data[i].size(); j++)
        {
            if(!hitboxes_data[i][j].Get_is_active())
            {
                continue;
            }

            auto aabb_a = hitboxes_data[i][j].Get_swept_aabb(scene);
            auto possible_colisions = grid.Get_entries_for_object( aabb_a );

            for(uint32_t k{0}; k < possible_colisions.size(); k++)
            {
                auto entries = possible_colisions[k];
                for(uint32_t l{0}; l < entries.size(); l++)
                {
                    if( !( (1u << entries[l].layer) & mask ) )
                    {
                        continue;
                    }
                    if(!hitboxes_data[ entries[l].layer ][ entries[l].id ].Get_is_active())
                    {
                        continue;
                    }
                    if(entries[l].layer == i)
                    {
                        if(entries[l].id == j) continue;
                        if(entries[l].id < j) continue;
                    }

                    auto aabb_b = hitboxes_data[ entries[l].layer ][ entries[l].id ].Get_swept_aabb(scene);

                    if(colision_visitor(aabb_a, aabb_b))
                    {
                         can_coliding.push_back( {.hitbox_a_id = j, .hitbox_b_id = entries[l].id, .hitbox_a_layer = i, .hitbox_b_layer = entries[l].layer} );
                    }
                }
            }
        }
    }

    auto key = [](const auto & p)
    {
        return std::tie(p.hitbox_a_layer, p.hitbox_a_id, p.hitbox_b_layer, p.hitbox_b_id);
    };

    std::sort(can_coliding.begin(), can_coliding.end(), [&](const auto & a, const auto & b) { return key(a) < key(b); });
    can_coliding.erase(std::unique(can_coliding.begin(), can_coliding.end(), [&](const auto & a, const auto & b) { return key(a) == key(b); }), can_coliding.end());

    last_can_coliding_size = can_coliding.size();

    colision_visitor.delta_time = delta_time;

    for(auto i{0UZ}; i < can_coliding.size(); i++)
    {
        if( std::visit(colision_visitor, hitboxes_data[can_coliding[i].hitbox_a_layer][can_coliding[i].hitbox_a_id].Get_hitbox(scene), hitboxes_data[can_coliding[i].hitbox_b_layer][can_coliding[i].hitbox_b_id].Get_hitbox(scene)) )
        {
            event::Event::Collision_occurred colision{};
            colision.object_a_id = hitboxes_data[can_coliding[i].hitbox_a_layer][can_coliding[i].hitbox_a_id].Get_render_object_id();
            colision.object_b_id = hitboxes_data[can_coliding[i].hitbox_b_layer][can_coliding[i].hitbox_b_id].Get_render_object_id();
            colision.object_a_layer = can_coliding[i].hitbox_a_layer;
            colision.object_b_layer = can_coliding[i].hitbox_b_layer;

            event_manager.Announce_event( colision );
        }
    }
}

void eruptor::physic::Physic_manager::On_event(const event::Event & event)
{
    if(auto model_change = event.Get_if<event::Event::Render_object_changed_model>())
    {
        for(auto & layer : hitboxes_data)
        {
            for(auto & hitbox_data : layer)
            {
                if(hitbox_data.Get_render_object_id() == model_change->render_object_id)
                {
                    hitbox_data.Set_model(*this, model_change->model_handle_id);
                }
            }
        }
    }
}
