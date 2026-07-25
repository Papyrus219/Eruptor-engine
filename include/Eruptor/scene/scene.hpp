#ifndef ERUPTOR_SCENE_SCENE_HPP
#define ERUPTOR_SCENE_SCENE_HPP

#include <Eruptor/scene/render_object.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace eruptor::scene
{

struct Scene
{
    std::unordered_map<std::string, size_t> objects_aliases{};
    std::vector<Render_object> render_objects{};
};

}

#endif // ERUPTOR_SCENE_SCENE_HPP
