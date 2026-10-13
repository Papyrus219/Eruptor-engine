#include <Eruptor/physic/grid.hpp>

using namespace eruptor::physic;

void eruptor::physic::Grid::Init(uint32_t rows_amount, uint32_t columns_amount, glm::vec3 map_min, glm::vec3 map_max, uint32_t average_amount_for_field)
{
    if(map_min.x < 0)
    {
        map_width = std::abs(map_min.x) + map_max.x;
    }
    else
    {
        map_width = map_max.x - map_min.x;
    }

    if(map_min.z < 0)
    {
        map_height = std::abs(map_min.z) + map_max.z;
    }
    else
    {
        map_height = map_max.z - map_min.z;

    }

    world_grid_width_diff = -map_min.x;
    world_grid_height_diff = -map_min.z;

    grid.resize(rows_amount);
    for(auto i{0UZ}; i < rows_amount; i++)
    {
        grid[i].resize(columns_amount);
        for(auto j{0UZ}; j < columns_amount; j++)
        {
            grid[i][j].reserve(average_amount_for_field);
        }
    }
}

void eruptor::physic::Grid::Set_map_size(glm::vec3 map_min, glm::vec3 map_max)
{
    if(map_min.x < 0)
    {
        map_width = std::abs(map_min.x) + map_max.x;
    }
    else
    {
        map_width = map_max.x - map_min.x;
    }

    if(map_min.z < 0)
    {
        map_height = std::abs(map_min.z) + map_max.z;
    }
    else
    {
        map_height = map_max.z - map_min.z;

    }

    world_grid_width_diff = -map_min.x;
    world_grid_height_diff = -map_min.z;
}
