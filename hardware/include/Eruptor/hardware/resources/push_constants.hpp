#ifndef ERUPTOR_HARDWARE_PUSH_CONSTANTS_HPP
#define ERUPTOR_HARDWARE_PUSH_CONSTANTS_HPP

#include <glm/glm.hpp>

namespace eruptor::hardware
{

struct Push_constant_opaque
{
    glm::mat4x4 model{1.0f};
    glm::vec4 color{};
};

struct Push_constant_debug
{
   alignas(16) glm::vec3 min{};
   alignas(16) glm::vec3 max{};
   alignas(16) glm::vec3 color{1.0f};
};

}

#endif // ERUPTOR_HARDWARE_PUSH_CONSTANT_HPP
