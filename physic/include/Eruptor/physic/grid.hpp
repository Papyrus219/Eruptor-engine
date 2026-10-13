#ifndef ERUPTOR_PHYSIC_GRID_HPP
#define ERUPTOR_PHYSIC_GRID_HPP

#include <Eruptor/physic/hitbox.hpp>
#include <vector>

namespace eruptor::physic
{

struct Tile
{
    uint32_t id{};
    uint8_t layer{};
};

class Grid
{
public:
    void Init(uint32_t rows_amount, uint32_t columns_amount, glm::vec3 map_min, glm::vec3 map_max, uint32_t average_amount_for_field = 2);

    void Set_map_size(glm::vec3 map_min, glm::vec3 map_max);

private:
    float map_width{}, map_height{}, world_grid_width_diff{}, world_grid_height_diff{};
    std::vector< std::vector<std::vector<uint32_t>> > grid{};
};

}

#endif // ERUPTOR_PHYSIC_GRID_HPP
