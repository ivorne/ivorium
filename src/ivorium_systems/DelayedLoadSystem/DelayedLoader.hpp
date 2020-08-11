#pragma once

#include "DelayedLoadSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class DelayedLoad;

/**
    Class that manages delayed loading of things that want to be loaded with delay.
    This is usualy part of a loading screen manager.
*/
class DelayedLoader
{
public:
ClientMarker cm;
                                DelayedLoader( Instance * inst );
                                ~DelayedLoader();
    
    Instance *                  instance() const;
    
    void                        BlockLoading( bool block );
    bool                        LoadingRunning();
    DelayedLoadStatus const &   LoadingStatus();
    
    /**
        One item of given complexity queued for load to system.
    */
    virtual void                LoadStart() = 0;
    
    /**
        System loaded one item that was previously queued. 
    */
    virtual void                LoadFinish() = 0;
    
    /**
    */
    virtual void                ItemQueued( DelayedLoad const * item, int complexity_bytes ){};
    
    /**
    */
    virtual void                ItemLoaded( DelayedLoad const * item, int complexity_bytes ){};

private:
    Instance * inst;
    DelayedLoadSystem * dls;
};

}
