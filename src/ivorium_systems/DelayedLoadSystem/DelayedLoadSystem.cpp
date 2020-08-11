#include "DelayedLoadSystem.hpp"
#include "DelayedLoader.hpp"
#include "../Defs.hpp"

namespace iv
{

DelayedLoadStatus::DelayedLoadStatus() :
    items_total( 0 ),
    items_done( 0 ),
    complexity_total( 0 ),
    complexity_done( 0 )
{
}

DelayedLoadSystem::DelayedLoadSystem( SystemContainer * sc ) :
    System( sc ),
    _to_load(),
    _loaders(),
    _blockers(),
    _loading( false ),
    status()
{
}

bool DelayedLoadSystem::loadStep()
{
    if( this->_blockers.size() )
        return false;
    
    if( !this->_to_load.size() )
        return false;
    
    if( !this->_loading )
    {
        this->_loading = true;
        this->Notify_LoadStart();
    }
    
    // extract from queue
    auto it = this->_to_load.begin();
    DelayedLoad * load = it->first;
    int complexity_bytes = it->second;
    this->_to_load.erase( it );
    
    // load
    load->LoadNow();
    
    // notify
    this->status.items_done += 1;
    this->status.complexity_done += complexity_bytes;
    this->Notify_ItemLoaded( load, complexity_bytes );
    
    // notify loading end
    if( this->_to_load.empty() )
    {
        this->status = DelayedLoadStatus();
        this->_loading = false;
        this->Notify_LoadFinish();
    }
    
    return true;
}

void DelayedLoadSystem::QueueDelayedLoad( DelayedLoad * load, int complexity_bytes )
{
    if( this->_loaders.empty() )
    {
        load->LoadNow();
        return;
    }
    
    this->_to_load[ load ] = complexity_bytes;
    this->status.items_total += 1;
    this->status.complexity_total += complexity_bytes;
    
    this->Notify_ItemQueued( load, complexity_bytes );
}

void DelayedLoadSystem::ClearDelayedLoad( DelayedLoad * load )
{
    this->_to_load.erase( load );
}

void DelayedLoadSystem::loader_register( DelayedLoader * loader )
{
    this->_loaders.insert( loader );
}

void DelayedLoadSystem::loader_unregister( DelayedLoader * loader )
{
    this->_loaders.erase( loader );
    this->_blockers.erase( loader );
}

void DelayedLoadSystem::BlockLoading( DelayedLoader * loader, bool block )
{
    if( !this->_loaders.count( loader ) )
        return;
    
    if( block )
        this->_blockers.insert( loader );
    else
        this->_blockers.erase( loader );
}

bool DelayedLoadSystem::LoadingRunning()
{
    return this->_loading;
}

void DelayedLoadSystem::Notify_LoadStart()
{
    this->log( SRC_INFO, Defs::Log::DelayedLoad, "Load start." );
    this->_loaders.foreach( [&]( DelayedLoader * const & loader )
    {
        loader->LoadStart();
    });
}

void DelayedLoadSystem::Notify_LoadFinish()
{
    this->log( SRC_INFO, Defs::Log::DelayedLoad, "Load finish." );
    this->_loaders.foreach( [&]( DelayedLoader * const & loader )
    {
        loader->LoadFinish();
    });
}

void DelayedLoadSystem::Notify_ItemQueued( DelayedLoad const * item, int complexity_bytes )
{
    this->log( SRC_INFO, Defs::Log::DelayedLoad, "Load queued for ",item->cm,"." );
    this->_loaders.foreach( [&]( DelayedLoader * const & loader )
    {
        loader->ItemQueued( item, complexity_bytes );
    });
}

void DelayedLoadSystem::Notify_ItemLoaded( DelayedLoad const * item, int complexity_bytes )
{
    this->log( SRC_INFO, Defs::Log::DelayedLoad, "Load performed for ",item->cm,"." );
    this->_loaders.foreach( [&]( DelayedLoader * const & loader )
    {
        loader->ItemLoaded( item, complexity_bytes );
    });
}

DelayedLoadStatus const & DelayedLoadSystem::Status()
{
    return this->status;
}

}
