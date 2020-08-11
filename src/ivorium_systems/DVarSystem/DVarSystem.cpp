#include "DVarSystem.hpp"

namespace iv
{

DVarSystem::DVarSystem( SystemContainer * sc ) :
    System( sc )
{
}

DVarSystem::~DVarSystem()
{
    for( auto & p_var : this->vars )
        if( p_var.second.val )
            p_var.second.deleter();
}

void DVarSystem::add_listener( DVarListenerI * listener, DVarId id )
{
    this->vars[ id ].listeners.insert( listener );
}

void DVarSystem::remove_listener( DVarListenerI * listener, DVarId id )
{
    this->vars[ id ].listeners.erase( listener );
}


void DVarSystem::lock( Instance * setter, DVarId id )
{
    this->vars[ id ].locked = setter;
}

void DVarSystem::unlock( Instance * setter, DVarId id )
{
    auto & var = this->vars[ id ];
    if( var.locked == setter )
        var.locked = nullptr;
    else
        this->warning( SRC_INFO, "Can not unlock DVar ", id, ". It is locked to different instance." );
}

Instance * DVarSystem::locked( DVarId id )
{
    return this->vars[ id ].locked;
}

}
