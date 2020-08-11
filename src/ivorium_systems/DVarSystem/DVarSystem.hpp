#pragma once

#include "DVarId.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <typeindex>

namespace iv
{

class DVarListenerI
{
public:
    /**
        \p value is reinterpret castable to type described by \p type
    */
    virtual void    on_dvar_changed( DVarId id, std::type_index type, void * value ) = 0;
};

/**
    NOTE - If we want debug access to existing dvar ids, then the list should be queried from RuntimeId dictionary (not implemented by the time of writing this note).
*/
class DVarSystem : public System
{
public:
    /**
        DVarSystem lets parent system decide on value of DVars that are not specified in this sytem.
    */
                    DVarSystem( SystemContainer * sc );
                    ~DVarSystem();
                    
    virtual std::string debug_name() const override { return "DVarSystem"; }
    
    /**
        Lock id so that given setter can change or unlock it.
    */
    void            lock( Instance * setter, DVarId id );
    
    /**
        Revert effect of DVarSystem::lock.
        Should be called only by the one who locked it.
    */
    void            unlock( Instance * setter, DVarId id );
    
    /**
        Return instance that has the id locked or nullptr, if it is not locked.
    */
    Instance *      locked( DVarId id );
    
    /**
        Will be changed only if dvar is not locked or if \p setter is the same one who locked it.
        If the write fails (due to wrong setter) then a warning will be printed.
    */
    template< class Type >
    void            set( Instance * setter, DVarIdT< Type > id, Type const & value );
    
    template< class Type >
    Type            get( DVarIdT< Type > id );
    
    void            add_listener( DVarListenerI * listener, DVarId id );
    void            remove_listener( DVarListenerI * listener, DVarId id );
    
private:
    struct Var
    {
        volatile_set< DVarListenerI * > listeners;
        std::type_index type;
        void * val;
        std::function< void() > deleter;
        Instance * locked;
        
        Var() : listeners(), type( typeid(void) ), val( nullptr ), deleter(), locked( nullptr ){}
    };
    
    std::unordered_map< DVarId, Var > vars;
};

}

#include "DVarSystem.inl"
