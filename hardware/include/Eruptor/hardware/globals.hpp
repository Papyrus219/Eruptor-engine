#ifndef ERUPTOR_HARDWARE_GLOBALS_HPP
#define ERUPTOR_HARDWARE_GLOBALS_HPP

namespace eruptor::hardware
{

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

}

#endif //ERUPTOR_HARDWARE_GLOBALS_HPP
