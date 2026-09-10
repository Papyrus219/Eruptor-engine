#ifndef ERUPTOR_PHYSIC_HITBOX_METADATA_HPP
#define ERUPTOR_PHYSIC_HITBOX_METADATA_HPP

#include <Eruptor/scene/scene.hpp>
#include <Eruptor/physic/hitbox.hpp>
#include <Eruptor/physic/hitbox_type.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <cstdint>

namespace eruptor::physic
{

class Physic_manager;

struct Hitbox_metadata
{
    void Set_model(Physic_manager & physic_manager, uint32_t model_resource_id);

    const AABB & Get_aabb(scene::Scene & scene);
    const Hitbox & Get_hitbox(scene::Scene & scene);
    eruptor::physic::AABB Get_swept_aabb(scene::Scene & scene);

    uint32_t Get_render_object_id() const {return this->render_object_id;}

private:
    uint32_t render_object_id{};

    std::optional<glm::vec3> position{};
    std::optional<glm::quat> rotation{};
    std::optional<glm::vec3> scale{};

    AABB model_aabb{};
    AABB transformed_aabb{};
    AABB last_aabb{};

    Hitbox_type type{};
    Hitbox model_hitbox{};
    Hitbox transformed_hitbox{};
};

}

#endif //ERUPTOR_PHYSIC_HITBOX_METADATA_HPP
