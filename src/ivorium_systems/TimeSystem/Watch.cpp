#include "Watch.hpp"

#include "TimeSystem.hpp"

namespace iv
{

Watch::Watch( Instance * inst, TimeId time_type ) :
    cm( inst, this, "Watch" ),
    inst( inst ),
    ts( inst->getSystemContainer()->getSystem< TimeSystem >() ),
    _time_type( time_type ),
    delta_cache( 0 ),
    timeout_it(),
    timeout_it_valid( false )
{
    if( !this->ts )
        return;
    
    if( !this->ts->is_valid( this->_time_type ) )
    {
        this->cm.warning( SRC_INFO, "This TimeId value (", this->_time_type.persistent_value(), ") can not be processed by TimeSystem. TimeId::create on this TimeId value was probably called after TimeSystem was initialized. TimeSystem can be changed to support this at cost of small performance decrease." );
        this->_time_type = TimeId();
    }
    
    this->delta_cache = this->ts->get_time_ms( this->_time_type );
}

Instance * Watch::instance()
{
    return this->inst;
}

TimeId Watch::time_type()
{
    return this->_time_type;
}

void Watch::time_type( TimeId tt )
{
    if( !this->ts )
        return;
        
    if( !this->ts->is_valid( tt ) )
    {
        this->cm.warning( SRC_INFO, "This TimeId value (", this->_time_type.persistent_value(), ") can not be processed by TimeSystem. TimeId::create on this TimeId value was probably called after TimeSystem was initialized. TimeSystem can be changed to support this at cost of small performance decrease." );
        tt = TimeId();
    }
    
    // adjust delta_cache to new time (so that next call to Watch::delta_ms gives sane value)
    int cache_change = this->ts->get_time_ms( this->_time_type ) - this->delta_cache;
    double real_cache_change = cache_change / this->ts->speed( this->_time_type );
    int new_cache_change = real_cache_change * this->ts->speed( tt );
    this->delta_cache = this->ts->get_time_ms( tt ) - new_cache_change;
    
    // set new 
    this->_time_type = tt;
}

int Watch::time_ms()
{
    if( this->ts )
        return this->ts->get_time_ms( this->_time_type );
    else
        return 0;
}

int Watch::delta_ms()
{
    if( this->ts )
    {
        int time = this->ts->get_time_ms( this->_time_type );
        int result = time - this->delta_cache;
        this->delta_cache = time;
        return result;
    }
    
    return 0;
}

void Watch::timeout( int delta_ms, std::function< void() > const & fun )
{
    if( this->ts )
        this->ts->timeout( this, this->_time_type, this->ts->get_time_ms( this->_time_type ) + delta_ms, fun );
}

void Watch::timeout_unique( int delta_ms, std::function< void() > const & fun )
{
    if( !this->ts )
        return;
    
    if( this->timeout_it_valid )
    {
        this->timeout_it_valid = false;
        this->ts->cancel_timeout( this->_time_type, this->timeout_it );
    }
    
    this->timeout_it_valid = true;
    this->timeout_it = this->ts->timeout( this, this->_time_type, this->ts->get_time_ms( this->_time_type ) + delta_ms, 
        [ this, fun ]()
        {
            this->timeout_it_valid = false;
            fun();
        }
    );
}

void Watch::timeout_unique()
{
    if( !this->ts )
        return;
    
    if( this->timeout_it_valid )
    {
        this->timeout_it_valid = false;
        this->ts->cancel_timeout( this->_time_type, this->timeout_it );
    }
}

void Watch::delay( std::function< void() > const & fun )
{
    if( this->ts )
        this->ts->delay( this, fun );
}

int Watch::time_to_next_frame_ms()
{
    if( this->ts )
        return this->ts->get_time_to_next_frame_ms( this->_time_type );
    else
        return 1;
}

}
