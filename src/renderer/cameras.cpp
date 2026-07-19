#include <Eruptor/renderer/cameras.hpp>

eruptor::renderer::Camera::Camera(glm::vec3 position_, glm::vec3 up_, float yaw_, float pitch_): position{position_}, front{0.0f, 0.0f, -1.0f}, world_up{up_}, yaw{yaw_}, pitch{pitch_}
{
    Update_camera_vectors();
}

void eruptor::renderer::Camera::Update_camera_vectors()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, world_up));
    up    = glm::normalize(glm::cross(right, front));
}

void eruptor::renderer::Fly_camera::Process_keyboard(Camera_movement_direction direction, float delta_time)
{
    float velocity = movement_speed * delta_time;

    switch(direction)
    {
        case Camera_movement_direction::FORWARD:
            position += front * velocity;
            break;
        case Camera_movement_direction::BACKWARD:
            position -= front * velocity;
            break;
        case Camera_movement_direction::LEFT:
            position -= right * velocity;
            break;
        case Camera_movement_direction::RIGHT:
            position += right * velocity;
            break;
    }
}

void eruptor::renderer::Fly_camera::Process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch)
{
    y_offset *= -1;
    x_offset *= mouse_sensivity;
    y_offset *= mouse_sensivity;

    yaw += x_offset;
    pitch += y_offset;

    if(constrain_pitch)
    {
        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
    }

    Update_camera_vectors();
}

void eruptor::renderer::Fly_camera::Process_mouse_scroll(float y_offset)
{
    zoom -= y_offset;
    if(zoom < 1.0f) zoom = 1.0f;
    if(zoom > 45.0f) zoom = 45.0f;
}

