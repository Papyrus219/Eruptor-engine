#ifndef ERUPTOR_SCENE_SCENE_HPP
#define ERUPTOR_SCENE_SCENE_HPP

#include <Eruptor/scene/render_object.hpp>
#include <vector>

namespace eruptor::scene
{

struct Scene
{
    std::vector<Render_object> render_objects{};
};

}

#endif // ERUPTOR_SCENE_SCENE_HPP
