#ifndef ERUPTOR_RENDERER_CAMERAS_HPP
#define ERUPTOR_RENDERER_CAMERAS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eruptor::renderer
{

enum class Camera_movement_direction
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    Camera(glm::vec3 position_ = {}, glm::vec3 up_ = {0.0f, 1.0f, 0.0f}, float yaw_ = -90.0f, float pitch_ =  {});

    glm::mat4 Get_view_matrix() {return glm::lookAt(position, position + front, up);}
    float Get_fov() {return zoom;}
    glm::vec3 Get_position() {return position;}
    glm::vec3 Get_front() {return front;}

protected:
    void Update_camera_vectors();

    glm::vec3 position{};
    glm::vec3 front{};
    glm::vec3 up{};
    glm::vec3 right{};
    glm::vec3 world_up{};

    float yaw{};
    float pitch{};
    float movement_speed{10.5f};
    float mouse_sensivity{0.1f};
    float zoom{45.0f};
};

class Fly_camera: public Camera
{
public:
    Fly_camera(glm::vec3 position_ = {}, glm::vec3 up_ = {0.0f, 1.0f, 0.0f}, float yaw_ = -90.0f, float pitch_ =  {}): Camera{position_, up_, yaw_, pitch_} {};

    void Process_keyboard(Camera_movement_direction direction , float delta_time);
    void Process_mouse_scroll(float y_offset);
    void Process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch = true);
};

}

#endif // ERUPTOR_RENDERER_FLY_CAMERA_HPP
