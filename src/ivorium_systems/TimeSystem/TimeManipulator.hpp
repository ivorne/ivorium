#pragma once

#include "TimeId.hpp"

namespace iv
{

class Instance;
class TimeSystem;

class TimeManipulator
{
public:
                            TimeManipulator( Instance * inst );
    Instance *              instance();
    
    /**
        Speed very close to 0 will stop frame updates.
        Positive speed will have frame updates in usual rate (not affected by speed).
        Negative speed is not allowed.
    */
    void                    speed( TimeId time_type, double speed );
    double                  speed( TimeId time_type );
    
    void                    game_update_period( TimeId time_type, int period_ms );
    int                     game_update_period( TimeId time_type );
    
private:
    Instance * inst;
    TimeSystem * ts;
};
    
}
