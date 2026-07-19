#ifndef ERUPTOR_HARDWARE_PUSH_CONSTANT_HPP
#define ERUPTOR_HARDWARE_PUSH_CONSTANT_HPP

#include <glm/glm.hpp>

namespace eruptor::hardware
{

struct Push_constant
{
    glm::mat4x4 model{1.0f};
    glm::vec4 skin_color{};
};

}

#endif // ERUPTOR_HARDWARE_PUSH_CONSTANT_HPP
