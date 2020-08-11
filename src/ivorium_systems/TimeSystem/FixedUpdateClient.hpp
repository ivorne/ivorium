#pragma once

#include "TimeId.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class FixedUpdateClient
{
public:
ClientMarker cm;

                            FixedUpdateClient( Instance * inst, TimeId time );
                            ~FixedUpdateClient();
    Instance *              instance() const;

    void                    fixed_update_pause();
    void                    fixed_update_resume();
    
    virtual void            fixed_update( TimeId time, int time_step, int steps ) = 0;

private:
    Instance * inst;
    TimeId time;
    bool active;
};



}
