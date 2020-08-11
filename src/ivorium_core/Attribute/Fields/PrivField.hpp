#pragma once

#include "../Field.hpp"

namespace iv
{
    
template< class T >
class PrivField;

//==================== PrivField_Owner =================================
template< class T >
class PrivField_Owner
{
public:
    virtual void                Field_OnChanged( Field< T > * field, bool real ){}
    
protected:
    void                        Field_Modify( PrivField< T > * field, T const & );
};


//==================== LField_RW =================================
template< class T >
class PrivField : public Field< T >
{
public:
friend class PrivField_Owner< T >;

                                PrivField( Instance * inst, PrivField_Owner< T > * impl );
    
protected:
    void                        Modify( T const & );
    
protected:
    virtual void                OnChanged( bool real ) override;
    
private:
    PrivField_Owner< T > * impl;
};

}

#include "PrivField.inl"
