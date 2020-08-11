#pragma once

#include "DVarSystem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
template< class Type >
class DVar_Field : public Field< Type >, private DVarListenerI
{
public:
ClientMarker cm;
                        DVar_Field( Instance * inst, DVarIdT< Type > id );
                        ~DVar_Field();

    Instance *          instance() const;
    
private:
    // DVarListenerI
    virtual void        on_dvar_changed( DVarId id, std::type_index type, void * value ) override;
    
    // Field< Type >
    virtual void        OnChanged( bool real ) override;
    
private:
    Instance * inst;
    DVarIdT< Type > id;
};

}

#include "DVar_Field.inl"
