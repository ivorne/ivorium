#include "ResourceProvider.hpp"
#include "ResourceManagementSystem.hpp"

namespace iv
{

ResourceProvider::ResourceProvider( Instance * inst, size_t priority ) :
    cm( inst, this, "ResourceProvider" ),
    inst( inst ),
    rms( inst->getSystem< ResourceManagementSystem >() )
{
    if( this->rms )
        this->rms->register_provider( this, priority );
}

ResourceProvider::~ResourceProvider()
{
    this->unregister_provider();
}

Instance * ResourceProvider::instance() const
{
    return this->inst;
}

void ResourceProvider::unregister_provider()
{
    if( this->rms )
        this->rms->unregister_provider( this );
}

}
