#pragma once

#include "DelayedLoad.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <unordered_map>

namespace iv
{

class DelayedLoader;

class DelayedLoadStatus
{
public:
    DelayedLoadStatus();

    int     items_total;
    int     items_done;
    int     complexity_total;
    int     complexity_done;
};

/**
*/
class DelayedLoadSystem : public System
{
public:
                            DelayedLoadSystem( SystemContainer * sc );
    virtual std::string     debug_name() const override { return "DelayedLoadSystem"; }
    
    bool                    loadStep();
    
    //--------------------- DelayedLoadSystem --------------------
    /**
        DelayedLoad
    */
    void                    QueueDelayedLoad( DelayedLoad * load, int complexity_bytes );
    void                    ClearDelayedLoad( DelayedLoad * load );
    
    /**
        DelayedLoader
    */
    void                    loader_register( DelayedLoader * );
    void                    loader_unregister( DelayedLoader * );
    void                    BlockLoading( DelayedLoader *, bool block );
    bool                    LoadingRunning();
    
    /**
        Loading status;
    */
    DelayedLoadStatus const & Status();
    
private:
    void                    Notify_LoadStart();
    void                    Notify_LoadFinish();
    void                    Notify_ItemQueued( DelayedLoad const * item, int complexity_bytes );
    void                    Notify_ItemLoaded( DelayedLoad const * item, int complexity_bytes );
    
private:
    std::unordered_map< DelayedLoad *, int > _to_load;
    
    volatile_set< DelayedLoader * > _loaders;
    volatile_set< DelayedLoader * > _blockers;
    
    bool _loading;
    DelayedLoadStatus status;
};

}
