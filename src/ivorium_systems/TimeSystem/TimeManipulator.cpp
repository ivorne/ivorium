#include "TimeManipulator.hpp"
#include "TimeSystem.hpp"

namespace iv
{

TimeManipulator::TimeManipulator( Instance * inst ) :
    inst( inst ),
    ts( inst->getSystemContainer()->getSystem< TimeSystem >() )
{
}

Instance * TimeManipulator::instance()
{
    return this->inst;
}

void TimeManipulator::speed( TimeId time_type, double speed )
{
    if( this->ts )
        this->ts->speed( time_type, speed );
}

double TimeManipulator::speed( TimeId time_type )
{
    if( this->ts )
        return this->ts->speed( time_type );
    else
        return 1.0;
}

void TimeManipulator::game_update_period( TimeId time_type, int period_ms )
{
    if( !this->ts )
        return;
    
    this->ts->game_update_period( time_type, period_ms );
}

int TimeManipulator::game_update_period( TimeId time_type )
{
    if( !this->ts )
        return 16;
    
    return this->ts->game_update_period( time_type );
}

}
