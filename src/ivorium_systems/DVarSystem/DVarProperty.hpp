#pragma once

#include "DVarId.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <optional>
#include <functional>

namespace iv
{

template< class T >
class DVarProperty : private DVarListenerI
{
public:
ClientMarker cm;

                    DVarProperty( Instance * inst, DVarIdT< T > id );
                    ~DVarProperty();
    
    /**
    */
    Instance *      instance();

    /**
        If empty value is set, then the value of this is determined by the DVar system.
        If non-empty value is set, then this non-empty value is used.
    */
    void            value( std::optional< T > const & val );
    
    /**
        Returns the value - either the value set by the setter (DVarProperty<T>::value) or value determined by DVar system.
        If DVarProperty<T>::value setter was not called before, then this will return value from DVar system.
    */
    T               value();

    /**
        Called whenever the result of DVarPropert::value getter changes, this is called.
        This will be called when it is changed either from dvar system or from value setter.
        If parameter 'call_immediately' is called, then the callback function is called from inside of this call to DVarProperty<T>::on_changed with current value.
    */
    void            on_changed( std::function< void( T const & newval ) > const &, bool call_immediately = false );
    
private:
    virtual void    on_dvar_changed( DVarId id, std::type_index type, void * value ) override;
    
private:
    Instance *          inst;
    DVarSystem *        dvs;
    DVarIdT< T >        _dvar_id;
    std::function< void( T const & newval ) > _on_changed;
    std::optional< T >  _explicit_value;
    T                   _cached_value;
    
};

}

#include "DVarProperty.inl"
