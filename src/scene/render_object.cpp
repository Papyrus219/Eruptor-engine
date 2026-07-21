#include <Eruptor/scene/render_object.hpp>
#include <Eruptor/resource_manager.hpp>
#include <numeric>

eruptor::scene::Render_object::Render_object(resource::Resource_manager & resource_manager, resource::Model_handle model_handle_): model_handle{model_handle_}, model_aabb{resource_manager.Get_model_aabb( model_handle )}, transformed_aabb{ model_aabb }
{

}

void eruptor::scene::Render_object::Set_model(resource::Resource_manager& resource_manager, resource::Model_handle model_handle)
{
    this->model_handle = model_handle;
    this->model_aabb = resource_manager.Get_model_aabb( model_handle );
    this->aabb_has_changed = true;
}

eruptor::physic::AABB eruptor::scene::Render_object::Get_aabb()
{
    if(aabb_has_changed)
    {

        glm::vec3 corners[8] =
        {
            {model_aabb.min.x, model_aabb.min.y, model_aabb.min.z},
            {model_aabb.max.x, model_aabb.min.y, model_aabb.min.z},
            {model_aabb.min.x, model_aabb.max.y, model_aabb.min.z},
            {model_aabb.max.x, model_aabb.max.y, model_aabb.min.z},

            {model_aabb.min.x, model_aabb.min.y, model_aabb.max.z},
            {model_aabb.max.x, model_aabb.min.y, model_aabb.max.z},
            {model_aabb.min.x, model_aabb.max.y, model_aabb.max.z},
            {model_aabb.max.x, model_aabb.max.y, model_aabb.max.z}
        };

        transformed_aabb = {glm::vec3{std::numeric_limits<float>::max()}, glm::vec3{std::numeric_limits<float>::lowest()}};

        for(auto& corner : corners)
        {
            glm::vec3 transformed =
            glm::vec3(transformation.Get_model_matrix() * glm::vec4(corner, 1.0f));

            transformed_aabb.min = glm::min(transformed_aabb.min, transformed);
            transformed_aabb.max = glm::max(transformed_aabb.max, transformed);
        }
    }

    return transformed_aabb;
}

//Transform interface

void eruptor::scene::Render_object::Set_position(glm::vec3 new_position)
{
    transformation.Set_position( new_position );

    aabb_has_changed = true;
}

void eruptor::scene::Render_object::Set_scale(glm::vec3 new_scale, std::optional<float> snap_y)
{
    transformation.Set_scale( new_scale );

    if(snap_y)
    {
        Snap_to_y(*snap_y);
    }

    aabb_has_changed = true;
}

void eruptor::scene::Render_object::Set_rotation_euler(glm::vec3 new_rotation)
{
    transformation.Set_rotation_euler( new_rotation );

    aabb_has_changed = true;
}

void eruptor::scene::Render_object::Move(glm::vec3 move_offset)
{
    transformation.Set_position( transformation.Get_position() + move_offset );

    aabb_has_changed = true;
}

void eruptor::scene::Render_object::Change_scale(glm::vec3 scale_offset, std::optional<float> snap_y)
{
    transformation.Set_scale( transformation.Get_scale() + scale_offset );

    if(snap_y)
    {
        Snap_to_y(*snap_y);
    }

    aabb_has_changed = true;
}

void eruptor::scene::Render_object::Rotate(glm::vec3 rotation_offset)
{
    glm::quat quat_offset = glm::quat(rotation_offset);
    transformation.Set_rotation_quad( transformation.Get_rotation() * quat_offset );

    aabb_has_changed = true;
}

void eruptor::scene::Render_object::Snap_to_y(float target_y)
{
    physic::AABB aabb = Get_aabb();
    glm::vec3 pos = transformation.Get_position();
    pos.y += target_y - aabb.min.y;
    transformation.Set_position( pos );

    aabb_has_changed = true;
}

