#ifndef ERUPTOR_PHYSIC_PHYSIC_MANAGER_HPP
#define ERUPTOR_PHYSIC_PHYSIC_MANAGER_HPP

#include <Eruptor/event/event_listener.hpp>
#include <Eruptor/physic/colision_visitor.hpp>
#include <Eruptor/physic/hitbox_calculator.hpp>
#include <Eruptor/physic/hitbox_metadata.hpp>
#include <Eruptor/physic/can_colide_info.hpp>
#include <vector>
#include <unordered_map>
#include <functional>

namespace eruptor::scene
{
    struct Scene;
}
namespace eruptor::event
{
    class Event_manager;
}

namespace eruptor::physic
{

class Physic_manager: public event::Event_listener
{
public:
    Physic_manager();

    ///@todo ADD HITBOX_TYPE IN SET HITBOX IN HITBOX METADATA

    void Update_scene(scene::Scene & scene, float delta_time);

    void On_event(const event::Event & event) override;

    void Add_hitbox(uint8_t layer, uint32_t render_id, scene::Scene & scene);
    void Add_model_hitbox(uint32_t model_resource_id, physic::Hitbox_type hitbox_type, std::vector<glm::vec3> & all_vertecies);

    std::optional<std::reference_wrapper<Hitbox_metadata>> Get_hitbox_data(uint8_t layer, uint32_t render_id);

    AABB Get_model_aabb(uint32_t model_resource_id) {return model_aabbs.at(model_resource_id);}
    Hitbox Get_model_hitbox(uint32_t model_resource_id) {return model_hitboxes.at(model_resource_id);}
    Hitbox_type Get_model_hitbox_type(uint32_t model_resource_id) {return model_hitboxes_types.at(model_resource_id);}

private:
    void Snap_y(scene::Scene & scene);
    void Chceck_colisions(scene::Scene & scene, float delta_time);

    static constexpr uint8_t LAYERS_AMOUNT{8};

    std::array<std::vector<Hitbox_metadata>, LAYERS_AMOUNT>  hitboxes_data{};
    std::array<std::vector< std::pair<uint32_t, AABB> >, LAYERS_AMOUNT> sweep_aabbs{};

    std::unordered_map<uint32_t, AABB> model_aabbs{};
    std::unordered_map<uint32_t, Hitbox> model_hitboxes{};
    std::unordered_map<uint32_t, Hitbox_type> model_hitboxes_types{};

    Colision_visitor colision_visitor{};
    Hitbox_calculator hitbox_calculator{};

    std::vector< Can_colide_info > can_coliding{};
    size_t last_can_coliding_size{};

    event::Event_manager & event_manager;
};

}

#endif // ERUPTOR_PHYSIC_PHYSIC_MANAGER_HPP
