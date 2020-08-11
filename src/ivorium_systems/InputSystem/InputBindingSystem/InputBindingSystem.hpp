#pragma once

#include "InputId.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

class InputBindingListener;

/**
    Note about HOLD inputs and MODIFIERS:
        - They should not be implemented in InputBindingSystem (because we might want to inform player about pending HOLD action or MODIFIER being pressed).
        - This features should be implemented in client code (descendants of InputEvent).
        - ActivationEvent could have bool attribute 'is_hold', which would automaticaly trigger notification after given time and notify some kind of HoldListener about pending hold (so that the listener can show progress).
        - Modifier can be implemented using a DurationEvent which enables and disables an ActivationEvent (which will signalize to player that it is active). Also a listener to this DurationEvent can signalize to player that modifier is being pressed. Switches (like CapsLock) can be implemented similarly, but using ActivationEvent that switches state (enabled/disabled) upon its activation.
        - It might be desirable to configure HOLD flag and MODIFIERS list in a user Keybinds settings class. That means that this Keybinds class will not only configure InputBindingSystem, but will also notify some other code that is responsible for managing modifiers and hold inputs. I could even make a small system that will just store this information (but it might be more semantic than just simple "InputId - HOLD, MODIFIERS" relation).
        - the point of HOLD not being integrated to the input system is that HOLD has meaning only for some InputEvents (DurationEvent does not care for holds). We can extend this notion and say that neither all ActivationEvents has to support this (at least partialy - they will probably support the hold activation, but the visual feedback depends on each implementation anyway and missing implementation for visual feedback seems to be a valid option)
        
    InputValues: (for analog triggers)
        They are not implemented yet (no input devices with analog inputs are implemented - mouse and touchscreen are considered 'positional' inputs, not 'analog' inputs here).
        They could be implemented as unordered_map from InputId to set of callable objects that retrieve the value (or a handle id that contains the information about where to get that info).
        The input_value() method will call all of them and get the greatest.
        Binding and unbinding controls will add and remove these callable objects.
        Analog inputs will add real objects that retrieve the value of the input, digital inputs will just return a value (either 1.0 or some other constant set by bind_*() methods).
        Or do it more similar to how positional info is done (multiple maps for multiple input types - in this case, it will only be controller key id or fixed number).
        
    Touch reordering:
        If you want to change the way touches are reordered when one of them is relased, modify method InputBindingSystem::touchEnd.
        
    Ještě dořešit - co když někdy chceme hráči signalizovat, že stiskl tlačítko, které je ale zrovna zakázané?
        - vpodstatě neřešit
        - většinou by tohle nemělo být moc potřeba
        - pokud je to potřeba, tak si to asi budou řešit jednotlivé implementace listenerů samy
        - pokud by se ukázalo, že to jde řešit nějak systémověji, tak by se to implementovalo někde na úrovní InputEventů
        - asi to prostě nechat z hlediska InputEventů jako aktivní (protože to stejně potřebuju zabrat ostatním InputEventům) a pouze implementace bude buď daná akce nebo signalizace neaktivity
    
    Ještě znovu zvážit jak řešit hover.
        - to si resi GuiItemEventy samy pres ten svuj system
*/
class InputBindingSystem : public System
{
public:
                        InputBindingSystem( SystemContainer * sc );
                        
    virtual std::string debug_name() const override { return "InputBindingSystem"; }
    virtual void        status( TextDebugView * view ) override;

    void                register_listener( InputBindingListener * );
    void                unregister_listener( InputBindingListener * );

    //----------------- called by InputBinding -----------------------------------------------
    /**
        Negative device_index means that all devices would be bound.
    */
    void                BindKey( InputId inputId, Input::Key key, int device_index = -1 );
    void                UnbindKey( InputId inputId, Input::Key key, int device_index = -1 );
    
    void                BindHoverKey( InputId inputId, Input::Key key, int device_index = -1 );
    void                UnbindHoverKey( InputId inputId, Input::Key key, int device_index = -1 );
    
    //----------------- called by InputEventClient -------------------------------------------------
    /**
        Returns true if key and device_index are bound to given InputId.
    */
    bool                IsBound( InputId inputId, Input::Key key, int device_index );
    
    /**
    */
    bool                IsHoverBound( InputId inputId, Input::Key key, int device_index );
    
    /**
        void Callable::operator()( Input::Key key, int device_id = -1 );
    */
    template< class Callable >
    void                ForeachHoverBinding( InputId inputId, Callable callable )
    {
        auto it = this->hover_binds.find( inputId );
        if( it == this->hover_binds.end() )
            return;
        
        Binds const & bind = it->second;
        
        for( auto const & dev_key : bind.device )
            callable( dev_key.first, dev_key.second );
        
        for( auto const & key : bind.all )
            callable( key, -1 );
    }
    
    /**
        void Callable::operator()( Input::Key key, int device_id );
        If something is bound to all devices, the device_id will be negative.
        NOTE - This is probably currently unused.
    */
    template< class Callable >
    void                ForeachBinding( InputId inputId, Callable callable )
    {
        auto it = this->binds.find( inputId );
        if( it == this->binds.end() )
            return;
        
        Binds const & bind = it->second;
        
        for( auto const & dev_key : bind.device )
            callable( dev_key.first, dev_key.second );
        
        for( auto const & key : bind.all )
            callable( key, -1 );
    }
    
private:
    void binding_changed();
    
private:
    struct Binds
    {
        std::unordered_set< Input::Key > all;
        std::unordered_set< Input::DeviceKey, Input::DeviceKey_Hash > device;
    };
    
    std::unordered_map< InputId, Binds > binds;
    std::unordered_map< InputId, Binds > hover_binds;
    
    volatile_set< InputBindingListener * > listeners;
};

}
