#ifndef ERUPTOR_RESOURCE_RESOURCE_HPP
#define ERUPTOR_RESOURCE_RESOURCE_HPP

#include <string>

namespace eruptor::resource
{

class Resource
{
public:
    explicit Resource(const std::string & id);
    virtual ~Resource() = default;

    const std::string & Get_id() const;
    bool Is_loaded() const;

    bool Load();
    void Unload();

private:
    std::string resource_id{};
    bool loaded{};

protected:
    virtual bool Do_load() = 0;
    virtual bool Do_unload() = 0;
};

}

#endif // ERUPTOR_RESOURCE_RESOURCE_HPP
