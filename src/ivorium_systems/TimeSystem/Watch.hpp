#pragma once

#include "TimeId.hpp"
#include "TimeSystem.hpp"
#include "../Defs.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <functional>

namespace iv
{


class TimeSystem;
class Instance;

class Watch
{
public:
ClientMarker cm;

                            Watch( Instance * inst, TimeId time_type = TimeId() );
    Instance *              instance();
    
    /**
     */
    void                    time_type( TimeId tt );
    TimeId                  time_type();
    
    /**
        Returns game time in milliseconds from when the TimeId started (this does not increase if TimeId used by this Watch is paused).
    */
    int                     time_ms();
    
    /**
        Returns delta time from last time Watch::delta_ms was called on this instance (or from when it was constructed when this is the first time it is called).
    */
    int                     delta_ms();
    
    /**
        TODO - tohle odebrat a dát sem nějaký mechanizmus, který zaručí, že některé timeouty budou maximálně jednou za nějaký čas (ne jednou za frame, ale jednou za nějaké simulační epsilon)
            Možná sem ale dát jenom nějaký helper, který pomůže klientskému kódu definovat si to časové epsilon sám. 
            Protože u některých simulací potřebujeme jiný minimální simulační krok než u ostatních - každá simulace je jinak akční a tedy jinak náchylná na chybějící kroky.
            Tohle se mimochodem používá zatím asi jenom v rising_sun/Behaviors/WorldPhysics/CentroidMotion.cpp.
    */
    int                     time_to_next_frame_ms();
    
    /**
        Called once after 'delta_ms' time passed.
        It is not called if this instance of Watch is destroyed before timeout.
    */
    void                    timeout( int delta_ms, std::function< void() > const & fun );
    
    /**
        Calling this second time will remove the previous timeout, so only one timeout_unique can be pending simultaneously.
    */
    void                    timeout_unique( int delta_ms, std::function< void() > const & fun );
    
    /**
        This cancels current timeout_unique.
    */
    void                    timeout_unique();
    
    /**
        Function is called once later in this frame.
        This is useful to avoid dangerous recursions.
        It is not called if this instance of Watch is destroyed before it is called.
    */
    void                    delay( std::function< void() > const & fun );
    
private:
    Instance *      inst;
    TimeSystem *    ts;
    TimeId          _time_type;
    int             delta_cache;
    
    TimeSystem::timeout_iterator    timeout_it;
    bool                            timeout_it_valid;
};

}
