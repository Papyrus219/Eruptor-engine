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
    void Set_render_object_id(uint32_t render_object_id);
    void Set_model(Physic_manager & physic_manager, uint32_t model_resource_id);

    void Set_individual_position(glm::vec3 position);
    void Set_individual_rotation(glm::quat rotation);
    void Set_individual_scale(glm::vec3 scale);

    void Set_is_active(bool is_active) {this->is_active = is_active;}

    uint32_t Get_render_object_id() const {return this->render_object_id;}
    bool Get_is_active() const {return this->is_active;}

    const Hitbox & Get_hitbox(scene::Scene & scene);
    Hitbox_type Get_hitbox_type() {return type;}
    const AABB & Get_aabb(scene::Scene & scene);
    AABB Get_swept_aabb(scene::Scene & scene);

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

    bool is_active{true};
};

}

#endif //ERUPTOR_PHYSIC_HITBOX_METADATA_HPP
