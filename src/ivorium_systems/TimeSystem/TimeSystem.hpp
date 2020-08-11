#pragma once

#include "FrameUpdateClient.hpp"
#include "TimeId.hpp"

#include <ivorium_core/ivorium_core.hpp>

#include <map>
#include <set>
#include <queue>
#include <unordered_map>
#include <array>

namespace iv
{
    
class Watch;
class FixedUpdateClient;

class TimeSystem : public System
{
public:
    struct UpdateItem
    {
        std::function< void() > fun;
        instance_ptr watch;
        
        UpdateItem( Instance * watch, std::function< void() > const & fun ) : fun( fun ), watch( watch ){}
    };

    using timeout_iterator = std::multimap< int, UpdateItem >::iterator;

    
public:
    // system
                                TimeSystem( SystemContainer * sc );
    virtual std::string         debug_name() const override { return "TimeSystem"; }
    virtual bool                flushSystem() override;
    
    // system control
    void                        addTime( int delta_ms );
    void                        nextFrame();
    
    // client interface
    int                         get_time_ms( TimeId time_type );
    int                         get_time_to_next_frame_ms( TimeId time_type );
    
    void                        register_frame_update_client( FrameUpdateClient * client );
    void                        unregister_frame_update_client( FrameUpdateClient * client );
    
    void                        register_fixed_update_client( FixedUpdateClient * client, TimeId time_type );
    void                        unregister_fixed_update_client( FixedUpdateClient * client, TimeId time_type );
    
    void                        speed( TimeId time_type, double speed );
    double                      speed( TimeId time_type );
    
    void                        game_update_period( TimeId time_type, int period_ms );
    int                         game_update_period( TimeId time_type );
    
    timeout_iterator            timeout( Watch * watch, TimeId time_type, int time_abs_ms, std::function< void() > const & fun );
    
    void                        cancel_timeout( TimeId time_type, timeout_iterator it );        ///< this can be called only if given iterator was returned from TimeSystem::timeout method and the timeout callback was not called yet; This functionality should be wrapped in a client class (eg. class Watch) to guarantee this behavior
    
    void                        delay( Watch * watch, std::function< void() > const & fun );

    bool                        is_valid( TimeId time_type );

private:
    struct TimeData
    {
        int game_update_period_ms;
        double speed_current;       // currently used speed
        double speed_target;        // speed to be set at the beginning of next frame
        int frame_start_time_ms;    // time in ms that was on this clock when the frame started
        double frame_start_time_remainder;  // frame_start_time_ms is integral type, so when speed_current is low, we might lose digits after the decimal point. This is the remainder after conversion to integral type. We will attempt to add this to start_time_ms the next frame.
        
        TimeData() : game_update_period_ms( 16 ), speed_current( 1.0 ), speed_target( 1.0 ), frame_start_time_ms( 0 ), frame_start_time_remainder( 0.0 ){}
    };
    
    int frame_time_ms;      // how far in frame are we, in real time
    int frame_length_ms;    // how much of real time to add to frame
    
    std::vector< TimeData > time_data;
    
    std::vector< std::multimap< int, UpdateItem > > timeout_queues;
    std::queue< UpdateItem > delayed;
    
    bool frame_update_queued;
    std::set< FrameUpdateClient * > frame_clients;
    std::set< FrameUpdateClient * >::iterator frame_clients_iterator;
    
    struct FixedData
    {
        int last_update;
        std::set< FixedUpdateClient * > clients;
        std::set< FixedUpdateClient * >::iterator iterator;
        
        FixedData() : last_update( 0 ), iterator( this->clients.end() ){}
    };
    
    std::vector< FixedData > fixed;
};

}
