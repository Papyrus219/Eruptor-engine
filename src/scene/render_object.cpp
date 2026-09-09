#include <Eruptor/scene/render_object.hpp>
#include <Eruptor/resource_manager.hpp>
#include <Eruptor/event/event_manager.hpp>
#include <numeric>

void eruptor::scene::Render_object::Reset()
{
    transformation.Reset();

    is_active = true;
}


void eruptor::scene::Render_object::Set_model(uint32_t object_id, resource::Model_handle model_handle)
{
    this->model_handle = model_handle;

    event::Event::Render_object_changed_model tmp_event{};
    tmp_event.render_object_id = object_id;
    tmp_event.model_handle_id = model_handle.Get_id();
    event::Event event = tmp_event;
    event::event_manager.Announce_event( event );

    this->model_changed = true;
    this->aabb_has_changed = true;
    this->hitbox_has_changed = true;
}

//Transform interface

void eruptor::scene::Render_object::Set_position(glm::vec3 new_position)
{
    transformation.Set_position( new_position );

    aabb_has_changed = true;
    hitbox_has_changed = true;

    reset_last_aabb = true;
    reset_last_hitbox = true;
}

void eruptor::scene::Render_object::Set_scale(glm::vec3 new_scale, std::optional<float> snap_y)
{
    transformation.Set_scale( new_scale );

    if(snap_y)
    {
        Snap_to_y(*snap_y);
    }

    aabb_has_changed = true;
    hitbox_has_changed = true;
}

void eruptor::scene::Render_object::Set_rotation_euler(glm::vec3 new_rotation)
{
    transformation.Set_rotation_euler( new_rotation );

    aabb_has_changed = true;
    hitbox_has_changed = true;
}

void eruptor::scene::Render_object::Set_rotation_quad(glm::quat new_rotation)
{
    transformation.Set_rotation_quad( new_rotation );

    aabb_has_changed = true;
    hitbox_has_changed = true;
}


void eruptor::scene::Render_object::Move(glm::vec3 move_offset)
{
    transformation.Set_position( transformation.Get_position() + move_offset );

    aabb_has_changed = true;
    hitbox_has_changed = true;
}

void eruptor::scene::Render_object::Change_scale(glm::vec3 scale_offset, std::optional<float> snap_y)
{
    transformation.Set_scale( transformation.Get_scale() + scale_offset );

    if(snap_y)
    {
        Snap_to_y(*snap_y);
    }

    aabb_has_changed = true;
    hitbox_has_changed = true;
}

void eruptor::scene::Render_object::Rotate(glm::vec3 rotation_offset)
{
    glm::quat quat_offset = glm::quat(rotation_offset);
    transformation.Set_rotation_quad( transformation.Get_rotation() * quat_offset );

    aabb_has_changed = true;
    hitbox_has_changed = true;
}

void eruptor::scene::Render_object::Snap_to_y(float target_y)
{
    physic::AABB aabb = Get_aabb();
    glm::vec3 pos = transformation.Get_position();
    pos.y += target_y - aabb.min.y;
    transformation.Set_position( pos );

    aabb_has_changed = true;
    hitbox_has_changed = true;
}

