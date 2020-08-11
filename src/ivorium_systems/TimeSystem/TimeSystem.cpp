#include "TimeSystem.hpp"
#include "Watch.hpp"
#include "FixedUpdateClient.hpp"

#include <limits>

namespace iv
{

TimeSystem::TimeSystem( SystemContainer * sc ) :
    System( sc ),
    frame_time_ms( 0 ),
    frame_length_ms( 0 ),
    frame_update_queued( false ),
    frame_clients_iterator( frame_clients.end() )
{
    this->time_data.resize( TimeId::ids_count() );
    this->timeout_queues.resize( TimeId::ids_count() );
    this->fixed.resize( TimeId::ids_count() );
    TimeId::lock();
}

void TimeSystem::addTime( int delta_ms )
{
    this->frame_length_ms += delta_ms;
}

void TimeSystem::nextFrame()
{
    this->frame_update_queued = true;
    
    for( TimeData & time : this->time_data )
    {
        double add = double( this->frame_time_ms ) * time.speed_current + time.frame_start_time_remainder;
        int add_int = int( add );
        
        time.frame_start_time_remainder = add - double( add_int );
        time.frame_start_time_ms += add_int;
        time.speed_current = time.speed_target;
    }
    
    this->frame_length_ms -= this->frame_time_ms;
    this->frame_time_ms = 0;
}

void TimeSystem::register_frame_update_client( FrameUpdateClient * client )
{
    this->frame_clients.insert( client );
}

void TimeSystem::unregister_frame_update_client( FrameUpdateClient * client )
{
    if( *this->frame_clients_iterator == client )
        ++this->frame_clients_iterator;
        
    this->frame_clients.erase( client );
}

int TimeSystem::get_time_ms( TimeId time_type )
{
    TimeData & time = this->time_data[ time_type.runtime_value() ];
    return time.frame_start_time_ms + int( time.speed_current * double( this->frame_time_ms ) );
}

int TimeSystem::get_time_to_next_frame_ms( TimeId time_type )
{
    TimeData & time = this->time_data[ time_type.runtime_value() ];
    return int( time.speed_current * double( this->frame_length_ms - this->frame_time_ms ) );
}

void TimeSystem::speed( TimeId time_type, double speed )
{
    TimeData & time = this->time_data[ time_type.runtime_value() ];
    time.speed_target = speed;
}

double TimeSystem::speed( TimeId time_type )
{
    TimeData & time = this->time_data[ time_type.runtime_value() ];
    return time.speed_target;
}


void TimeSystem::game_update_period( TimeId time_type, int period_ms )
{
    TimeData & time = this->time_data[ time_type.runtime_value() ];
    time.game_update_period_ms = period_ms;
}

int TimeSystem::game_update_period( TimeId time_type )
{
    TimeData & time = this->time_data[ time_type.runtime_value() ];
    return time.game_update_period_ms;
}

bool TimeSystem::is_valid( TimeId time_type )
{
    return time_type.runtime_value() >= 0 && (size_t)time_type.runtime_value() < this->time_data.size();
}

TimeSystem::timeout_iterator TimeSystem::timeout( Watch * watch, TimeId time_type, int time_abs_ms, std::function< void() > const & fun )
{
    return this->timeout_queues[ time_type.runtime_value() ].insert( std::make_pair( time_abs_ms, UpdateItem( watch->instance(), fun ) ) );
}

void TimeSystem::cancel_timeout( TimeId time_type, timeout_iterator it )
{
    this->timeout_queues[ time_type.runtime_value() ].erase( it );
}

void TimeSystem::delay( Watch * watch, std::function< void() > const & fun )
{
    this->delayed.push( UpdateItem( watch->instance(), fun ) );
}

void TimeSystem::register_fixed_update_client( FixedUpdateClient * client, TimeId time_type )
{
    this->fixed[ time_type.runtime_value() ].clients.insert( client );
}

void TimeSystem::unregister_fixed_update_client( FixedUpdateClient * client, TimeId time_type )
{
    auto & data = this->fixed[ time_type.runtime_value() ];
    
    if( *data.iterator == client )
        ++data.iterator;
    
    data.clients.erase( client );
}

bool TimeSystem::flushSystem()
{
    bool something_was_called = false;
    
    //---------- frame updates ------------------------
    if( this->frame_update_queued )
    {
        something_was_called = true;
        this->frame_update_queued = false;
        
        // update frame clients
        this->frame_clients_iterator = this->frame_clients.begin();
        while( this->frame_clients_iterator != this->frame_clients.end() )
        {
            FrameUpdateClient * client = *this->frame_clients_iterator;
            ++this->frame_clients_iterator;
            client->frame_update();
        }
    }
    
    //----------- fixed step updates -----------------------
    static const constexpr int FixedStepMs = 16;
    
    for( size_t i = 0; i < this->time_data.size(); i++ )
    {
        FixedData & data = this->fixed[ i ];
        int time_ms = this->time_data[ i ].frame_start_time_ms;
        
        if( data.last_update == 0 )
        {
            data.last_update = time_ms;
            continue;
        }
        
        int steps = ( time_ms - data.last_update ) / FixedStepMs;
        if( steps <= 0 )
            continue;
        
        something_was_called = true;
        
        data.last_update += steps * FixedStepMs;
        
        data.iterator = data.clients.begin();
        while( data.iterator != data.clients.end() )
        {
            FixedUpdateClient * client = *data.iterator;
            ++data.iterator;
            client->fixed_update( TimeId( i ), FixedStepMs, steps );
        }
    }
    
    //----------- update timeouts -----------------
    std::vector< double > real_frame_times_for_next_update;
    real_frame_times_for_next_update.resize( this->time_data.size() );
    for( size_t i=0; i < this->time_data.size(); i++ )
    {
        if( this->timeout_queues[ i ].empty() )
        {
            real_frame_times_for_next_update[ i ] = std::numeric_limits< double >::infinity();
            continue;
        }
        
        TimeData & time_data = this->time_data[ i ];
        int const & first_item_time = this->timeout_queues[ i ].begin()->first;
        
        real_frame_times_for_next_update[ i ] = double( first_item_time - time_data.frame_start_time_ms ) / time_data.speed_current;
    }
    
    while( true )
    {
        // find clock that need to be updated fastest
        int nearest_clock = -1;
        double nearest_clock_value = std::numeric_limits< double >::infinity();
        for( size_t i=0; i<TimeId::ids_count(); i++ )
        {
            double frame_proximity = real_frame_times_for_next_update[ i ];
            if( frame_proximity < nearest_clock_value )
            {
                nearest_clock_value = frame_proximity;
                nearest_clock = i;
            }
        }
        
        // nothing to update was found
        if( nearest_clock == -1 )
        {
            this->frame_time_ms = this->frame_length_ms;
            break;
        }
        
        // there is something in update queue, but it should be updated in future
        if( int( nearest_clock_value ) >= this->frame_length_ms )
        {
            this->frame_time_ms = this->frame_length_ms;
            break;
        }
        
        // something will be updated
        something_was_called = true;
        
        // 
        this->frame_time_ms = int( nearest_clock_value );
        
        // retrieve the nearest update item
        std::multimap< int, UpdateItem > & queue = this->timeout_queues[ nearest_clock ];
        UpdateItem & update_item = queue.begin()->second;
        std::function< void() > update_fun;
        if( update_item.watch )
            update_fun = update_item.fun;
        queue.erase( queue.begin() );
        
        // update real_frame_times_for_next_update array
        if( this->timeout_queues[ nearest_clock ].empty() )
        {
            real_frame_times_for_next_update[ nearest_clock ] = std::numeric_limits< double >::infinity();
        }
        else
        {
            TimeData & time_data = this->time_data[ nearest_clock ];
            int const & first_item_time = this->timeout_queues[ nearest_clock ].begin()->first;
            real_frame_times_for_next_update[ nearest_clock ] = double( first_item_time - time_data.frame_start_time_ms ) / time_data.speed_current;
        }
        
        // call update
        if( update_fun )
            update_fun();
    }
    
    //--------- call delayed code --------------
    if( !this->delayed.empty() )
        something_was_called = true;
    
    while( !this->delayed.empty() )
    {
        UpdateItem & item = this->delayed.front();
        std::function< void() > fun;
        if( item.watch )
            fun = item.fun;
        this->delayed.pop();
        if( fun )
            fun();
    }
    
    //------- return ---------------------
    return something_was_called;
}

}
