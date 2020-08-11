#include "FixedUpdateClient.hpp"
#include "TimeSystem.hpp"
#include "../Defs.hpp"

namespace iv
{

FixedUpdateClient::FixedUpdateClient( Instance * inst, TimeId time ) :
    cm( inst, this, "FixedUpdateClient" ),
    inst( inst ),
    time( time ),
    active( true )
{
    auto ts = this->instance()->getSystem< TimeSystem >();
    if( ts )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Stop fixed update." );
        ts->register_fixed_update_client( this, this->time );
    }
}

FixedUpdateClient::~FixedUpdateClient()
{
    auto ts = this->instance()->getSystem< TimeSystem >();
    if( ts )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Stop fixed update." );
        ts->unregister_fixed_update_client( this, this->time );
    }
}

Instance * FixedUpdateClient::instance() const
{
    return this->inst;
}

void FixedUpdateClient::fixed_update_pause()
{
    if( !this->active )
        return;
    
    this->active = false;
    
    auto ts = this->instance()->getSystem< TimeSystem >();
    if( ts )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Stop fixed update." );
        ts->unregister_fixed_update_client( this, this->time );
    }
}

void FixedUpdateClient::fixed_update_resume()
{
    if( this->active )
        return;
    
    this->active = true;
    
    auto ts = this->instance()->getSystem< TimeSystem >();
    if( ts )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Start fixed update." );
        ts->register_fixed_update_client( this, this->time );
    }
}

}
