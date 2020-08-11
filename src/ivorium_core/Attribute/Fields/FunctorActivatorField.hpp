#pragma once

#include "../Types/Activator.hpp"
#include "../Field.hpp"
#include <functional>

namespace iv
{

/**
*/
class FunctorActivatorField : public Field< Activator >
{
public:
ClientMarker cm;
    
                        FunctorActivatorField( Instance * inst, std::function< void() > const & on_activated = nullptr );
    
    void                on_activated( std::function< void() > const & );
    
protected:
    virtual void        OnChanged( bool real ) override;
    
private:
    Activator _reference;
    std::function< void() > _on_activated;
};

}
