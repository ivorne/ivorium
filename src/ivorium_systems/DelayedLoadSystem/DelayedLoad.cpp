#include "DelayedLoadSystem.hpp"
#include "../Defs.hpp"

namespace iv
{

DelayedLoad::DelayedLoad( Instance * inst ) :
    cm( inst, this, "DelayedLoad" ),
    inst( inst )
{
}

DelayedLoad::~DelayedLoad()
{
    auto * sys = this->instance()->getSystem< DelayedLoadSystem >();
    if( sys )
        sys->ClearDelayedLoad( this );
}

Instance * DelayedLoad::instance() const
{
    return this->inst;
}

void DelayedLoad::LoadNow()
{
    this->Load();
}

void DelayedLoad::RequestDelayedLoad( int complexity_bytes )
{
    auto * sys = this->instance()->getSystem< DelayedLoadSystem >();
    if( sys )
        sys->QueueDelayedLoad( this, complexity_bytes );
}

}
