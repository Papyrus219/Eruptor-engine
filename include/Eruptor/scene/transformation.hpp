#ifndef ERUPTOR_SCENE_TRANSFORMATION_HPP
#define ERUPTOR_SCENE_TRANSFORMATION_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace eruptor::scene
{

class Transformation
{
public:
    [[nodiscard]] const glm::mat4x4 & Get_model_matrix();

    [[nodiscard]] glm::vec3 Get_position() { return position; }
    [[nodiscard]] glm::quat Get_rotation() { return rotation; }
    [[nodiscard]] glm::vec3 Get_scale() {return scale; }

    void Set_position(glm::vec3 position) {this->position = position; has_changed = true;}
    void Set_rotation_euler(glm::vec3 rotation) {this->rotation = glm::quat(rotation); has_changed = true;}
    void Set_rotation_quad(glm::quat rotation) {this->rotation = rotation;}
    void Set_scale(glm::vec3 scale) {this->scale = scale; has_changed = true;}

    bool Get_is_has_changed() {return has_changed;}

private:
    bool has_changed{true};

    glm::vec3 position{};
    glm::quat rotation{};
    glm::vec3 scale{1.0f};

    glm::mat4x4 last_model_matrix{};
};

}

#endif // ERUPTOR_SCENE_TRANSFORMATION_HPP
