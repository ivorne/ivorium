#include "DebugInstanceListener.hpp"

namespace iv
{

DebugInstanceListener::DebugInstanceListener( Instance * inst, InstanceSystem * target ) :
    inst( inst ),
    os( target )
{
    if( this->os )
        this->os->AddDebugListener( this );
}

DebugInstanceListener::~DebugInstanceListener()
{
    if( this->os )
        this->os->RemoveDebugListener( this );
}

Instance * DebugInstanceListener::instance() const
{
    return this->inst;
}

}
