#ifndef ERUPTOR_PHYSIC_CAN_COLIDE_INFO_HPP
#define ERUPTOR_PHYSIC_CAN_COLIDE_INFO_HPP

#include <cstdint>

namespace eruptor::physic
{

struct Can_colide_info
{
    uint32_t hitbox_a_id{};
    uint32_t hitbox_b_id{};

    uint8_t hitbox_a_layer{};
    uint8_t hitbox_b_layer{};
};

}

#endif //ERUPTOR_PHYSIC_CAN_COLIDE_INFO_HPP
