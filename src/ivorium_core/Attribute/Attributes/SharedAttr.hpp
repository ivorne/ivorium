#pragma once

#include "../Attribute.hpp"

namespace iv
{

/**
    MAttr and GSAttr are designed to be used in models.
    Shared Attribute and local listeners storage.
*/
template< class T >
class SharedAttr_I : public Attr< T >
{
public:
                    SharedAttr_I( ClientMarker * cm, AttributeEventProcessor *, T const & initial_value = T() );
    
protected:
    virtual void    GetSourceValue( T & out ) const override;
    virtual void    ModifySource( T const & ) override;
    
private:
    T value;
};

/**
    SharedAttr with locally stored listeners but unable to be copied, assigned or moved.
*/
template< class T >
class SharedAttr final : private Local_AEP, public SharedAttr_I< T >
{
public:
                    SharedAttr( ClientMarker * cm, T const & initial_value = T() );
                    ~SharedAttr();
};

}

#include "SharedAttr.inl"
