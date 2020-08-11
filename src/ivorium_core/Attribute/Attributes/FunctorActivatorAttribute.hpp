#pragma once

#include "Local_AEP.hpp"
#include <functional>
#include "../Types/Activator.hpp"

namespace iv
{

class FunctorActivatorAttribute final : private Local_AEP, public Attr< Activator >
{
public:
                            FunctorActivatorAttribute( ClientMarker * cm, std::function< void() > const & on_activated = nullptr );
                            ~FunctorActivatorAttribute();
                            
    void                    on_activated( std::function< void() > const & );
    void                    SetAttributeMode( Attribute::ValueMode mode );
    
protected:
    virtual void            GetSourceValue( Activator & out ) const override;
    virtual void            ModifySource( Activator const & ) override;
    
private:
    Activator activator;
    std::function< void() > _on_activated;
};


}
