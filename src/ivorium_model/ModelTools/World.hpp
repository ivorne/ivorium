#pragma once

#include <ivorium_core/ivorium_core.hpp>
#include <unordered_set>

namespace iv
{

/**
    \ingroup ModelTools
*/
template< class Component >
class World
{
public:
iv::ClientMarker cm;

                            World( iv::Instance * inst );
    iv::Instance *          instance() const;
    
    void                    Register( Component * );
    void                    Unregister( Component * );
    
    std::unordered_set< Component * > const & components();
    
protected:
    virtual void Component_Registered( Component * component ) = 0;
    virtual void Component_Unregistered( Component * component ) = 0;
    
private:
    iv::Instance * inst;
    std::unordered_set< Component * > _components;
};

}

#include "World.inl"
