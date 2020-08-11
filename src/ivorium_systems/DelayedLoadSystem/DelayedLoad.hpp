#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class DelayedLoadSystem;

/**
    Thing that has parts that should be loaded with delay (eg. Image wants its Texture to be loaded in delay - during loading screen).
*/
class DelayedLoad
{
public:
ClientMarker cm;

                        DelayedLoad( Instance * inst );
                        ~DelayedLoad();
    Instance *          instance() const;
    
    void                LoadNow();
    
protected:
    /**
        Tells the system that we want something to be loaded in the future.
    */
    void                RequestDelayedLoad( int complexity_bytes );
    
    virtual void        Load() = 0;
    
private:
    Instance * inst;
};

}
