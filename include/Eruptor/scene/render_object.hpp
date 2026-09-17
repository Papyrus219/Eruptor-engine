#ifndef ERUPTOR_SCENE_RENDER_OBJECT_HPP
#define ERUPTOR_SCENE_RENDER_OBJECT_HPP

#include <Eruptor/scene/transformation.hpp>
#include <Eruptor/resource/colors.hpp>
#include <Eruptor/resource/resource_handle.hpp>
#include <Eruptor/physic/hitbox.hpp>
#include <Eruptor/resource/model.hpp>
#include <optional>

namespace eruptor::scene
{

enum class Shading_type: uint8_t
{
    OPAQUE,
    LIGHT_CASTER
};

class Scene_parser;

struct Render_object
{
    void Reset();

    void Set_model(uint32_t object_id, resource::Model_handle model_handle);

    resource::Model_handle Get_model_handle() const {return model_handle;}

    //Transformation interface
    void Set_position(glm::vec3 new_position);
    void Set_scale(glm::vec3 new_scale);
    void Set_rotation_euler(glm::vec3 new_rotation);
    void Set_rotation_quad(glm::quat new_rotation);

    void Move(glm::vec3 move_offset);
    void Change_scale(glm::vec3 scale_offset);
    void Rotate(glm::vec3 rotation_offset);

    [[nodiscard]] glm::vec3 Get_position() const {return transformation.Get_position();}
    [[nodiscard]] glm::vec3 Get_scale() const {return transformation.Get_scale();}
    [[nodiscard]] glm::quat Get_rotation() const {return transformation.Get_rotation();}
    [[nodiscard]] const glm::mat4x4 & Get_model_matrix() {return transformation.Get_model_matrix();}

    resource::Color color{};
    uint32_t parent_object_index{}; ///NOTE Curently not in use
    Shading_type shading_type{};

    std::optional<float> snap_y{};

    bool is_selected{};
    bool is_active{true};

    uint64_t transformation_version{};
    uint64_t reset_version{};

    bool model_changed{};

private:
    Transformation transformation{};
    resource::Model_handle model_handle{};

    friend class Scene_parser;
};

}

#endif // ERUPTOR_SCENE_RENDER_OBJECT_HPP
