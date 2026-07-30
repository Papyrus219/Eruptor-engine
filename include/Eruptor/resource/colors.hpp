#ifndef PAP_VULC_COLOR_HPP
#define PAP_VULC_COLOR_HPP

#include <cstdint>
#include <glm/glm.hpp>

namespace eruptor::resource
{

struct Color
{
    constexpr Color() = default;
    constexpr Color(uint8_t red_, uint8_t green_, uint8_t blue_, uint8_t alpha_ = 255) : red{red_}, green{green_}, blue{blue_}, alpha{alpha_} {};

    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
    uint8_t alpha{};

    static float Srgb_to_linear(float color)
    {
        return (color <= 0.04045f) ? (color / 12.92f) : std::pow((color + 0.055f) / 1.055f, 2.4f);
    }

    operator glm::vec3() const
    {
        float piece{ 1.0f / 255.0f };
        return { Srgb_to_linear(piece * red), Srgb_to_linear(piece * green), Srgb_to_linear(piece * blue) };
    }

    operator glm::vec4() const
    {
        float piece{ 1.0f / 255.0f };
        return { Srgb_to_linear(piece * red), Srgb_to_linear(piece * green), Srgb_to_linear(piece * blue), piece * alpha };
    }

};



}

#endif //PAP_VULC_COLOR_HPP
