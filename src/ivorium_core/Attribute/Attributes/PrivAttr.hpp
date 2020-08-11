#pragma once

#include "Local_AEP.hpp"
#include "../Attribute.hpp"

namespace iv
{

template< class T >
class PrivAttr_I;

/**
*/
template< class T >
class PrivAttr_Owner
{
public:
    virtual void        PrivAttr_Get( PrivAttr_I< T > *, T & out ) = 0;
    virtual void        PrivAttr_Modify( PrivAttr_I< T > *, T const & value ) = 0;
    
protected:
    void                Attribute_SetMode( PrivAttr_I< T > *, Attribute::ValueMode mode );
    void                SourceValueChanged( PrivAttr_I< T > * );
};

/**
    Private attribute - controlled only through PrivAttr_Owner instance given in constructor, may allow (possibly only some) external modifications.
*/
template< class T >
class PrivAttr_I : public Attr< T >
{
public:
friend class PrivAttr_Owner< T >;
    
                        PrivAttr_I( ClientMarker *, AttributeEventProcessor *, PrivAttr_Owner< T > * owner );
    
protected:
    virtual void        GetSourceValue( T & out ) const override;
    virtual void        ModifySource( T const & ) const override;
    
private:
    PrivAttr_Owner< T > * owner;
};

/**
*/
template< class T >
class PrivAttr final : private Local_AEP, public PrivAttr_I< T >
{
public:
                        PrivAttr( ClientMarker *, PrivAttr_Owner< T > * owner );
                        ~PrivAttr();
};

}

#include "PrivAttr.inl"
