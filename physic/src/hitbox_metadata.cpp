#include <Eruptor/physic/hitbox_metadata.hpp>

const eruptor::physic::Hitbox & eruptor::physic::Hitbox_metadata::Get_hitbox(scene::Scene & scene)
{
    auto render_object = scene.render_objects[ this->render_object_id ];

    if(render_object.Get_is_hitbox_changed())
    {
        auto position = (this->position.has_value())? this->position.value() : render_object.Get_position();
    }

    return transformed_hitbox;
}
