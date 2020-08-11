#pragma once

#include "../Attribute.hpp"

namespace iv
{

template< class T >
class PrivValAttr_I;

/**
*/
class PrivValAttr_Owner
{
public:
    virtual void        PrivValAttr_Modified( Attribute * ){}
    
protected:
    template< class T >
    void                Attribute_Set( PrivValAttr_I< T > *, T const & );
    template< class T >
    void                Attribute_SetMode( PrivValAttr_I< T > *, Attribute::ValueMode );
};

/**
*/
template< class T >
class PrivValAttr_I : public Attr< T >
{
public:
friend class PrivValAttr_Owner;
    
                        PrivValAttr_I( ClientMarker * cm, AttributeEventProcessor *, PrivValAttr_Owner * owner, T const & initial_value = T() );
    
protected:
    virtual void        GetSourceValue( T & out ) const override;
    virtual void        ModifySource( T const & ) override;
    void                Set( T const & value );
    
private:
    PrivValAttr_Owner * owner;
    T value;
};

/**
*/
template< class T >
class PrivValAttr final: private Local_AEP, public PrivValAttr_I< T >
{
public:
                        PrivValAttr( ClientMarker * cm, PrivValAttr_Owner * owner, T const & initial_value = T() );
                        ~PrivValAttr();
};

}

#include "PrivValAttr.inl"
