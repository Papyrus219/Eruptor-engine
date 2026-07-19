#include <Eruptor/resource/resource.hpp>

eruptor::resource::Resource::Resource(const std::string & id): resource_id{id}
{

}

inline const std::string & eruptor::resource::Resource::Get_id() const
{
    return resource_id;
}

inline bool eruptor::resource::Resource::Is_loaded() const
{
    return loaded;
}

bool eruptor::resource::Resource::Load()
{
    loaded = Do_load();
    return loaded;
}

void eruptor::resource::Resource::Unload()
{
    Do_unload();
    loaded = false;
}
