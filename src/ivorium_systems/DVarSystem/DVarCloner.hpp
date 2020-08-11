#pragma once

#include "DVarListener.hpp"
#include "DVarId.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Locks target DVar so that only he can modify it and clones the value of source DVar every time it changes.
*/
template< class T >
class DVarCloner : private DVarListenerI
{
private:
ClientMarker cm;
    
                    DVarCloner( Instance * inst, DVarIdT< T > source, DVarIdT< T > target );
                    ~DVarCloner();
                
    Instance *      instance();
    
private:
    virtual void    on_dvar_changed( DVarId id, std::type_index type, void * value ) override;
    
private:
    Instance * inst;
    DVarSystem * dvs;
    DVarIdT< T > source;
    DVarIdT< T > target;
};

}

#include "DVarCloner.inl"
