#include "DelayedLoader.hpp"
#include "DelayedLoadSystem.hpp"

namespace iv
{

DelayedLoader::DelayedLoader( Instance * inst ) :
    cm( inst, this, "DelayedLoader" ),
    inst( inst ),
    dls( inst->getSystem< DelayedLoadSystem >() )
{
    if( this->dls )
        this->dls->loader_register( this );
}

DelayedLoader::~DelayedLoader()
{
    if( this->dls )
        this->dls->loader_unregister( this );
}

Instance * DelayedLoader::instance() const
{
    return this->inst;
}

bool DelayedLoader::LoadingRunning()
{
    if( this->dls )
        return this->dls->LoadingRunning();
    
    return false;
}

void DelayedLoader::BlockLoading( bool block )
{
    if( this->dls )
        this->dls->BlockLoading( this, block );
}

DelayedLoadStatus const & DelayedLoader::LoadingStatus()
{
    if( this->dls )
        return this->dls->Status();
    static DelayedLoadStatus status;
    return status;
}

}
