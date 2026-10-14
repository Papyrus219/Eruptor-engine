#include <Eruptor/physic/grid.hpp>

using namespace eruptor::physic;

void eruptor::physic::Grid::Init(uint32_t rows_amount, uint32_t columns_amount, glm::vec3 map_min, glm::vec3 map_max, uint32_t average_amount_for_field)
{
    Set_grid_size(rows_amount, columns_amount, average_amount_for_field);
    Set_map_size(map_min, map_max);
}

void eruptor::physic::Grid::Add_object_to_grid(const AABB & aabb, uint32_t id, uint8_t layer)
{
    const auto & min = aabb.min;
    const auto & max = aabb.max;

    int32_t min_row = static_cast<int32_t>( std::floor((min.z + world_grid_height_diff) / cell_size) );
    int32_t max_row = static_cast<int32_t>( std::floor((max.z + world_grid_height_diff) / cell_size) );
    int32_t min_column = static_cast<int32_t>( std::floor((min.x + world_grid_width_diff) / cell_size) );
    int32_t max_column = static_cast<int32_t>( std::floor((max.x + world_grid_width_diff) / cell_size) );

    min_row    = std::max(min_row, 0);
    max_row    = std::min(max_row, int32_t(rows_amount) - 1);
    min_column = std::max(min_column, 0);
    max_column = std::min(max_column, int32_t(columns_amount) - 1);

    if(min_row > max_row || min_column > max_column) return;

    for(auto i{min_row}; i <= max_row; i++)
    {
        for(auto j{min_column}; j <= max_column; j++)
        {
            grid[i][j].push_back( {id, layer} );
        }
    }
}

std::vector<std::span<const Entry>> eruptor::physic::Grid::Get_entries_for_object(const AABB & aabb)
{
    std::vector< std::span<const Entry> > result{};

    const auto & min = aabb.min;
    const auto & max = aabb.max;

    int32_t min_row = static_cast<int32_t>( std::floor((min.z + world_grid_height_diff) / cell_size) );
    int32_t max_row = static_cast<int32_t>( std::floor((max.z + world_grid_height_diff) / cell_size) );
    int32_t min_column = static_cast<int32_t>( std::floor((min.x + world_grid_width_diff) / cell_size) );
    int32_t max_column = static_cast<int32_t>( std::floor((max.x + world_grid_width_diff) / cell_size) );

    min_row    = std::max(min_row, 0);
    max_row    = std::min(max_row, int32_t(rows_amount) - 1);
    min_column = std::max(min_column, 0);
    max_column = std::min(max_column, int32_t(columns_amount) - 1);

    if(min_row > max_row || min_column > max_column) return {};

    for(auto i{min_row}; i <= max_row; i++)
    {
        for(auto j{min_column}; j <= max_column; j++)
        {
            result.emplace_back( grid[i][j] );
        }
    }

    return result;
}

void eruptor::physic::Grid::Set_map_size(glm::vec3 map_min, glm::vec3 map_max)
{
    map_width = map_max.x - map_min.x;
    map_height = map_max.z - map_min.z;

    world_grid_width_diff = -map_min.x;
    world_grid_height_diff = -map_min.z;

    cell_size = map_width / columns_amount;
}

void eruptor::physic::Grid::Set_grid_size(uint32_t rows_amount, uint32_t columns_amount, uint32_t average_amount_for_field)
{
    this->rows_amount = rows_amount;
    this->columns_amount = columns_amount;
    this->average_amount_for_field = average_amount_for_field;

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

void eruptor::physic::Grid::Clear()
{
    for(auto i{0UZ}; i < grid.size(); i++)
    {
        for(auto j{0UZ}; j < grid[i].size(); j++)
        {
            grid[i][j].clear();
        }
    }
}

