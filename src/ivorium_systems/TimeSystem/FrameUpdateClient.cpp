#include "FrameUpdateClient.hpp"
#include "TimeSystem.hpp"
#include "../Defs.hpp"

namespace iv
{

FrameUpdateClient::FrameUpdateClient( Instance * inst ) :
    cm( inst, this, "FrameUpdateClient" ),
    inst( inst ),
    ts( inst->getSystemContainer()->getSystem< TimeSystem >() ),
    active( 1 )
{
    if( this->ts )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Start frame update." );
        this->ts->register_frame_update_client( this );
    }
}

FrameUpdateClient::~FrameUpdateClient()
{
    if( this->ts )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Stop frame update." );
        this->ts->unregister_frame_update_client( this );
    }
}

Instance * FrameUpdateClient::instance()
{
    return this->inst;
}

void FrameUpdateClient::frame_update_pause()
{
    if( !this->ts )
        return;
    
    if( this->active > 0 )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Stop frame update." );
        this->ts->unregister_frame_update_client( this );
    }
        
    this->active = 0;
}

void FrameUpdateClient::frame_update_resume()
{
    if( !this->ts )
        return;
        
    if( this->active <= 0 )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Start frame update." );
        this->ts->register_frame_update_client( this );
    }
        
    this->active = 1;
}

bool FrameUpdateClient::frame_update_active()
{
    return this->active > 0;
}


void FrameUpdateClient::frame_update_inc()
{
    if( !this->ts )
        return;
        
    if( this->active == 0 )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Start frame update." );
        this->ts->register_frame_update_client( this );
    }
        
    this->active++;
}

void FrameUpdateClient::frame_update_dec()
{
    if( !this->ts )
        return;
        
    if( this->active == 1 )
    {
        this->cm.log( SRC_INFO, Defs::Log::TimeUpdates, "Stop frame update." );
        this->ts->unregister_frame_update_client( this );
    }
    
    this->active--;
}

}
