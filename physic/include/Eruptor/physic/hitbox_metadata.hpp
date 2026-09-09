#ifndef ERUPTOR_PHYSIC_HITBOX_METADATA_HPP
#define ERUPTOR_PHYSIC_HITBOX_METADATA_HPP

#include <Eruptor/scene/scene.hpp>
#include <Eruptor/physic/hitbox.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <cstdint>

namespace eruptor::physic
{

struct Hitbox_metadata
{
    ///@todo Implement Set_model

    const AABB & Get_aabb(scene::Scene & scene);
    const Hitbox & Get_hitbox(scene::Scene & scene);
    eruptor::physic::AABB Get_swept_aabb(scene::Scene & scene);

private:
    uint32_t render_object_id{};

    std::optional<glm::vec3> position{};
    std::optional<glm::quat> rotation{};
    std::optional<glm::vec3> scale{};

    physic::AABB model_aabb{};
    physic::AABB transformed_aabb{};
    physic::AABB last_aabb{};

    resource::Hitbox_type type{};
    Hitbox model_hitbox{};
    Hitbox transformed_hitbox{};
};

}

#endif //ERUPTOR_PHYSIC_HITBOX_METADATA_HPP
