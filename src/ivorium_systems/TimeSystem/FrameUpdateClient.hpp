#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class Instance;
class TimeSystem;

/**
    Update counter: if > 0, then frame_update will be called once per frame.
    Update counter is set to 1 by default, so if you want to use frame_update_inc and frame_update_dec, you might want to call frame_update_pause after FrameUpdateClient construction.
*/
class FrameUpdateClient
{
public:
ClientMarker cm;

                            FrameUpdateClient( Instance * inst );
                            ~FrameUpdateClient();
    Instance *              instance();

    virtual void            frame_update() = 0;
    
    void                    frame_update_pause();   ///< sets update counter to 0
    void                    frame_update_resume();  ///< sets update counter to 1
    
    void                    frame_update_inc();     ///< increases update counter (we will be updating)
    void                    frame_update_dec();     ///< decreases update counter (vote against frame updates, only call this if frame_update_inc was called before)
    
    bool                    frame_update_active();
    
private:
    Instance *      inst;
    TimeSystem *    ts;
    int             active;
};

}
