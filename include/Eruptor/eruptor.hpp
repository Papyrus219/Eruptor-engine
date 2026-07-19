#ifndef ERUPTOR_ERUPTOR_HPP
#define ERUPTOR_ERUPTOR_HPP

#include <Eruptor/renderer.hpp>
#include <Eruptor/resource_manager.hpp>
#include <Eruptor/hardware/window.hpp>
#include <memory>

namespace eruptor
{

namespace hardware
{
    class Hardware;
}

class Eruptor
{
public:
    Eruptor();
    ~Eruptor();

    void Init();

    renderer::Renderer & Get_renderer() {return renderer;}
    resource::Resource_manager & Get_resource_manager() {return resource_manager;}

private:
    std::unique_ptr<hardware::Hardware> hardware;

    resource::Resource_manager resource_manager{};
    renderer::Renderer renderer{};
};

}

#endif // ERUPTOR_ERUPTOR_HPP
