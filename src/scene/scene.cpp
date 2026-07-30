#include <Eruptor/scene/scene.hpp>

eruptor::scene::Scene::Scene(const Scene & other): objects_aliases{other.objects_aliases}, render_objects{other.render_objects}
{

}

eruptor::scene::Scene::Scene(Scene && other): objects_aliases{ std::move( other.objects_aliases ) }, render_objects{ std::move( other.render_objects ) }
{

}

eruptor::scene::Scene & eruptor::scene::Scene::operator=(const Scene & other)
{
    this->objects_aliases = other.objects_aliases;
    this->render_objects = other.render_objects;

    return *this;
}

eruptor::scene::Scene & eruptor::scene::Scene::operator=(Scene && other)
{
    this->objects_aliases = std::move( other.objects_aliases );
    this->render_objects = std::move( other.render_objects );

    return *this;
}
