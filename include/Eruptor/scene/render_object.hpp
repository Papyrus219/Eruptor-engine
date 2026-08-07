#ifndef ERUPTOR_SCENE_RENDER_OBJECT_HPP
#define ERUPTOR_SCENE_RENDER_OBJECT_HPP

#include <Eruptor/scene/transformation.hpp>
#include <Eruptor/resource/colors.hpp>
#include <Eruptor/resource/resource_handle.hpp>
#include <Eruptor/physic/hitbox.hpp>
#include <Eruptor/resource/model.hpp>
#include <optional>

namespace eruptor::resource
{
    class Resource_manager;
}

namespace eruptor::scene
{

class Scene_parser;

struct Render_object
{
    void Set_model(resource::Resource_manager & resource_manager, resource::Model_handle model_handle);

    physic::AABB Get_aabb();
    physic::Hitbox Get_hitbox();
    resource::Model_handle Get_model_handle() const {return model_handle;}

    //Transformation interface
    void Set_position(glm::vec3 new_position);
    void Set_scale(glm::vec3 new_scale, std::optional<float> snap_y = {});
    void Set_rotation_euler(glm::vec3 new_rotation);
    void Set_rotation_quad(glm::quat new_rotation);

    void Move(glm::vec3 move_offset);
    void Change_scale(glm::vec3 scale_offset, std::optional<float> snap_y = {});
    void Rotate(glm::vec3 rotation_offset);

    [[nodiscard]] glm::vec3 Get_position() const {return transformation.Get_position();}
    [[nodiscard]] glm::vec3 Get_scale() const {return transformation.Get_scale();}
    [[nodiscard]] glm::quat Get_rotaion() const {return transformation.Get_rotation();}
    [[nodiscard]] const glm::mat4x4 & Get_model_matrix() {return transformation.Get_model_matrix();}

    resource::Color color{};
    uint32_t parent_object_index{}; ///NOTE Curently not in use
    bool is_selected{};

private:
    void Snap_to_y(float target_y);

    Transformation transformation{};
    resource::Model_handle model_handle{};
    physic::AABB model_aabb{};
    physic::AABB transformed_aabb{};

    resource::Hitbox_type hitbox_type{};
    physic::Hitbox model_hitbox{};
    physic::Hitbox transformed_hitbox{};

    bool aabb_has_changed{};
    bool hitbox_has_changed{};

    friend class Scene_parser;
};

}

#endif // ERUPTOR_SCENE_RENDER_OBJECT_HPP
