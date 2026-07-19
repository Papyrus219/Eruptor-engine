#include <Eruptor/eruptor.hpp>
#include <Eruptor/hardware.hpp>
#include <Eruptor/hardware/memory_units.hpp>

using namespace eruptor;

eruptor::Eruptor::Eruptor()
{

}


void eruptor::Eruptor::Init()
{
    hardware = std::make_unique<hardware::Hardware>();
    hardware->Init();
    resource_manager.Init( hardware->Get_resource_manager() );
    renderer.Init(*hardware, resource_manager);
}

eruptor::Eruptor::~Eruptor()
{

}
