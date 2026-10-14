#ifndef ERUPTOR_PHYSIC_GRID_HPP
#define ERUPTOR_PHYSIC_GRID_HPP

#include <Eruptor/physic/hitbox.hpp>
#include <vector>
#include <span>

namespace eruptor::physic
{

struct Entry
{
    uint32_t id{};
    uint8_t layer{};
};

class Grid
{
public:
    void Init(uint32_t rows_amount, uint32_t columns_amount, glm::vec3 map_min, glm::vec3 map_max, uint32_t average_amount_for_field = 2);

    void Add_object_to_grid(const AABB & aabb, uint32_t id, uint8_t layer);
    std::vector< std::span<const Entry> > Get_entries_for_object(const AABB & aabb);

    void Set_map_size(glm::vec3 map_min, glm::vec3 map_max);
    void Set_grid_size(uint32_t rows_amount, uint32_t columns_amount, uint32_t average_amount_for_field = 2);

    void Clear();

private:
    float map_width{}, map_height{}, world_grid_width_diff{}, world_grid_height_diff{}, cell_size{};
    int32_t rows_amount{}, columns_amount{}, average_amount_for_field{};

    std::vector< std::vector<std::vector<Entry>> > grid{};
};

}

#endif // ERUPTOR_PHYSIC_GRID_HPP
