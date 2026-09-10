#include <Eruptor/physic/hitbox_metadata.hpp>
#include <Eruptor/physic_manager.hpp>

void eruptor::physic::Hitbox_metadata::Set_model(Physic_manager & physic_manager, uint32_t model_resource_id)
{
    model_hitbox = physic_manager.Get_model_hitbox( model_resource_id );
    model_aabb = physic_manager.Get_model_aabb( model_resource_id );
}

const eruptor::physic::AABB & eruptor::physic::Hitbox_metadata::Get_aabb(scene::Scene & scene)
{
    auto render_object = scene.render_objects[ this->render_object_id ];

    if(render_object.aabb_has_changed)
    {
        auto position = (this->position.has_value())? this->position.value() : render_object.Get_position();
        auto rotation = (this->rotation.has_value())? this->rotation.value() : render_object.Get_rotation();
        auto scale = (this->scale.has_value())? this->scale.value() : render_object.Get_scale();

        glm::mat4x4 model{1.0f};
        model = glm::translate(model, position);
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);

        auto old_aabb = transformed_aabb;

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
            glm::vec3 transformed = glm::vec3(model * glm::vec4(corner, 1.0f));

            transformed_aabb.min = glm::min(transformed_aabb.min, transformed);
            transformed_aabb.max = glm::max(transformed_aabb.max, transformed);
        }

        if(render_object.reset_last_aabb)
        {
            last_aabb = transformed_aabb;
            render_object.reset_last_aabb = false;
        }
        else
        {
            last_aabb = old_aabb;
        }

        render_object.aabb_has_changed = false;
    }

    return transformed_aabb;
}

const eruptor::physic::Hitbox & eruptor::physic::Hitbox_metadata::Get_hitbox(scene::Scene & scene)
{
    auto render_object = scene.render_objects[ this->render_object_id ];

    if(render_object.hitbox_has_changed)
    {
        auto position = (this->position.has_value())? this->position.value() : render_object.Get_position();
        auto rotation = (this->rotation.has_value())? this->rotation.value() : render_object.Get_rotation();
        auto scale = (this->scale.has_value())? this->scale.value() : render_object.Get_scale();

        glm::mat4x4 model{1.0f};
        model = glm::translate(model, position);
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);

        if(type == physic::Hitbox_type::SPHERE)
        {
            auto new_sphere = std::get<physic::Sphere_hitbox>(model_hitbox) * model;
            auto & old_sphere = std::get<physic::Sphere_hitbox>(transformed_hitbox);

            if(render_object.reset_last_hitbox)
            {
                new_sphere.last_center = new_sphere.center;
            }
            else
            {
                new_sphere.last_center = old_sphere.center;
            }

            transformed_hitbox = new_sphere;
        }
        else if(type == physic::Hitbox_type::OBB)
        {
            transformed_hitbox = std::get<OBB_hitbox>(model_hitbox) * model;
        }
        else if(type == physic::Hitbox_type::CAPSULE)
        {
            transformed_hitbox = std::get<Sphere_hitbox>(model_hitbox) * model;
        }

        render_object.hitbox_has_changed = false;
        render_object.reset_last_hitbox = false;
    }

    return transformed_hitbox;
}

eruptor::physic::AABB eruptor::physic::Hitbox_metadata::Get_swept_aabb(scene::Scene & scene)
{
    physic::AABB current_aabb = Get_aabb(scene);
    physic::AABB last_aabb = this->last_aabb;

    return {glm::min(last_aabb.min, current_aabb.min), glm::max(last_aabb.max, current_aabb.max)};
}
