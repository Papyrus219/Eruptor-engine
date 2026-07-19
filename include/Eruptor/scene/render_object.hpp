#ifndef ERUPTOR_SCENE_RENDER_OBJECT_HPP
#define ERUPTOR_SCENE_RENDER_OBJECT_HPP

#include <Eruptor/scene/transformation.hpp>
#include <Eruptor/resource/resource_handle.hpp>

namespace eruptor::scene
{

struct Render_object
{
    Transformation transformation{};
    glm::vec4 color{1.0f};

    uint32_t parent_object_index{};
    resource::Model_handle model_handle{};
};

}

#endif // ERUPTOR_SCENE_RENDER_OBJECT_HPP
