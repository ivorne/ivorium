#pragma once

#include "InputBindingSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class InputBindingQuery
{
public:
ClientMarker cm;
                        InputBindingQuery( Instance * inst );
                        ~InputBindingQuery();
    Instance *          instance() const;

    bool                IsBound( InputId inputId, Input::DeviceKey device_key );
    
    bool                IsHoverBound( InputId inputId, Input::DeviceKey device_key );
    
    /**
        void Callable::operator()( Input::Key key, int device_id );
        If something is bound to all devices, the device_id will be negative.
    */
    template< class Callable >
    void                ForeachBinding( InputId inputId, Callable callable )
    {
        if( !this->ies )
            return;
        this->ies->ForeachBinding( inputId, callable );
    }
    
    /**
        void Callable::operator()( Input::Key key, int device_id = -1 );
    */
    template< class Callable >
    void                ForeachHoverBinding( InputId inputId, Callable callable )
    {
        if( !this->ies )
            return;
        this->ies->ForeachHoverBinding( inputId, callable );
    }
    
private:
    Instance * inst;
    InputBindingSystem * ies;
};

}
