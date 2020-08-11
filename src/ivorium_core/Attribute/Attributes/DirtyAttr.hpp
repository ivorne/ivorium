#pragma once

#include "Local_AEP.hpp"
#include "../Attribute.hpp"

namespace iv
{

/**
    DirtyAttr is mostly designed to be used in view.
    Always in mode ValueModify.
*/
template< class T >
class DirtyAttr_I : public Attr< T >
{
public:
                        DirtyAttr_I( ClientMarker *, AttributeEventProcessor *, T const & initial_value = T() );
    bool                operator<( DirtyAttr_I< T > const & right ) const;
    
    /**
    */
    bool                dirty() const;
    
    /**
        Returns true if dirty flag was set before it was cleared.
    */
    bool                clear_dirty();
    
    /**
        Read-write rules are usually pretty specific for DirtyAttr, so we can allow modification without giving modifier ClientMarker (as is required by Attr<T>::Modify).
    */
    void                Set( T const & );
    
private:
    // Attribute_RW
    virtual void        GetSourceValue( T & out ) const override final;
    virtual void        ModifySource( T const & ) override final;
    
private:
    T _value;
    bool _dirty;
};

/**
*/
template< class T >
class DirtyAttr final : private Local_AEP, public DirtyAttr_I< T >
{
public:
    DirtyAttr( ClientMarker *, T const & initial_value = T() );
    ~DirtyAttr();
};


template< class T >
struct StringIO< DirtyAttr_I< T > >
{
    DirtyAttr_I< T > Read( const char * name, Context const * context )
    {
        return DirtyAttr_I< T >( StringIO< T >().Read( name, context ) );
    }
    
    std::string Write( DirtyAttr_I< T > const & value, Context const * context ) const
    {
        auto result = StringIO< T >().Write( value.Get(), context );
        if( value.dirty() )
            result += " (dirty)";
        return result;
    }
};

}

#include "DirtyAttr.inl"
