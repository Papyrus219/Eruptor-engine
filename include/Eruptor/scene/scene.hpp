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
    Scene() = default;
    Scene(const Scene & other);
    Scene(Scene && other);

    Scene & operator=(const Scene & other);
    Scene & operator=(Scene && other);

    std::unordered_map<std::string, size_t> objects_aliases{};
    std::unordered_map<size_t, std::string_view> reverse_object_aliases{};
    std::vector<Render_object> render_objects{};
};

}

#endif // ERUPTOR_SCENE_SCENE_HPP
