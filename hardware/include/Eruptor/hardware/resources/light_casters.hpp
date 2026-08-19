#ifndef ERUPTOR_HARDWARE_LIGHT_CASTERS_HPP
#define ERUPTOR_HARDWARE_LIGHT_CASTERS_HPP

#include <glm/glm.hpp>

namespace eruptor::hardware
{


struct Direction_caster
{
    alignas(16) glm::vec4 dirr{};

    alignas(16) glm::vec4 ambient{};
    alignas(16) glm::vec4 diffuse{};
    alignas(16) glm::vec4 specular{};
};

struct Point_caster
{
    alignas(16) glm::vec3 pos{};

    alignas(16) glm::vec3 ambient{};
    float constant{};
    alignas(16) glm::vec3 diffuse{};
    float linear{};
    alignas(16) glm::vec3 specular{};
    float quadratic{};
};

}

#endif //ERUPTOR_HARDWARE_LIGHT_CASTERS_HPP
