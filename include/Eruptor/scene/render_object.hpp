#ifndef ERUPTOR_SCENE_RENDER_OBJECT_HPP
#define ERUPTOR_SCENE_RENDER_OBJECT_HPP

#include <Eruptor/scene/transformation.hpp>
#include <Eruptor/resource/resource_handle.hpp>
#include <Eruptor/physic/colision_box.hpp>
#include <optional>

namespace eruptor::resource
{
    class Resource_manager;
}

namespace eruptor::scene
{

struct Render_object
{
    Render_object(resource::Resource_manager & resource_manager, resource::Model_handle model_handle_);

    void Set_model(resource::Resource_manager & resource_manager, resource::Model_handle model_handle);

    physic::AABB Get_aabb();
    resource::Model_handle Get_model_handle() {return model_handle;}

    //Transformation interface
    void Set_position(glm::vec3 new_position);
    void Set_scale(glm::vec3 new_scale, std::optional<float> snap_y = {});
    void Set_rotation_euler(glm::vec3 new_rotation);

    void Move(glm::vec3 move_offset);
    void Change_scale(glm::vec3 scale_offset, std::optional<float> snap_y = {});
    void Rotate(glm::vec3 rotation_offset);

    [[nodiscard]] glm::vec3 Get_position() {return transformation.Get_position();}
    [[nodiscard]] glm::vec3 Get_scale() {return transformation.Get_scale();}
    [[nodiscard]] glm::quat Get_rotaion() {return transformation.Get_rotation();}
    [[nodiscard]] const glm::mat4x4 & Get_model_matrix() {return transformation.Get_model_matrix();}

    glm::vec4 color{1.0f};
    uint32_t parent_object_index{}; ///NOTE Curently not in use
    bool is_coliding{};

private:
    void Snap_to_y(float target_y);

    Transformation transformation{};
    resource::Model_handle model_handle{};
    physic::AABB model_aabb{};
    physic::AABB transformed_aabb{};
    bool aabb_has_changed{};
};

}

#endif // ERUPTOR_SCENE_RENDER_OBJECT_HPP
