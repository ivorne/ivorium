#pragma once

#include "DVarSystem.hpp"

namespace iv
{

/**
    Used to access DVar value.
    To be notified about DVar changes, use DVarListener.
    TODO
        - if on_changed is enabled, then allow value caching
        - we need to modify DVarSystem so that he gives us value in the callback (void* reinterpret castable to the type - if it is known); posibly send type_index with it too
        - possible give this caching to DVarProperty
*/
template< class Type >
class DVar : private DVarListenerI
{
public:
ClientMarker cm;

                    DVar( Instance * inst, DVarIdT< Type > id = DVarIdT< Type >() );
                    ~DVar();
                    
    Instance *      instance();
                
    Type            get();
    
    void            dvar( DVarIdT< Type > id );
    DVarIdT< Type > dvar() const;
    
    void            on_changed( std::function< void( Type const & newval ) > const &, bool call_immediately = false );
    
private:
    virtual void    on_dvar_changed( DVarId id, std::type_index type, void * value ) override;
    
    Type            get_uncached();
    
private:
    Instance * inst;
    DVarSystem * dvs;
    DVarIdT< Type > id;
    std::function< void( Type const & newval ) > _on_changed;
    Type cached; ///< Contains value (synced using on_dvar_changed) if _on_changed is specified (an therefore listener is registered).
};

}

#include "DVar.inl"
