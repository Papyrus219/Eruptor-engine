#ifndef ERUPTOR_HARDWARE_PIPELINE_ID_HPP
#define ERUPTOR_HARDWARE_PIPELINE_ID_HPP

#include <cstdint>

namespace eruptor::hardware
{

enum class Pipeline_id: uint8_t
{
    OPAQUE,
    DEBUG,
    COUNT
};

}

#endif //ERUPTOR_HARDWARE_PIPELINE_ID_HPP
