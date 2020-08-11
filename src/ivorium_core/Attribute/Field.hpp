#pragma once

#include "Attribute.hpp"

#include "../Basics/valgrind.hpp"

#include <any>
#include <optional>

namespace iv
{

//================================== FieldI ==========================================
class FieldI
{
public:
    enum class Assignment
    {
        Unassigned,
        Constant,
        Attribute_R,
        Attribute_RW
    };
    
public:
                                    FieldI(){}
                                    FieldI( FieldI const & ) = delete;
    FieldI &                        operator=( FieldI const & ) = delete;
    
//--------------------------------- generic access ---------
    void                            Assign_AnyAttribute_R( Attribute * );
    void                            Assign_AnyAttribute_RW( Attribute * );
    
    std::type_index                 Type();
    Attribute::ValueMode            Mode() const;
    Assignment                      AssignmentState() const;
    
private:
    virtual void                    AnySource_R_Impl( Attribute * ) = 0;
    virtual void                    AnySource_RW_Impl( Attribute * ){}
    virtual std::type_index         Type_Impl() = 0;
    
    virtual Attribute::ValueMode    mode_Impl() const = 0;
    virtual Assignment              AssignmentState_impl() const = 0;
};


//================================== Field ========================================
template< class T >
class Field : public FieldI, private AttributeListener
{
public:
ClientMarker cm;

                                    Field( Instance * inst );
                                    ~Field();
    
    Instance *                      instance() const;
    
    /**
        Releases currently assigned attribute or constant value.
    */
    void                            Release_Attribute();
    
    /**
        Sets mode to Mode::Value.
        Releases any previously assigned attributes and constant values.
    */
    void                            Assign_Attribute_R( Attr< T > * );
    
    /**
        Sets mode to Mode::ValueModify.
        Releases any previously assigned attributes and constant values.
    */
    void                            Assign_Attribute_RW( Attr< T > * );

    /**
    */
    void                            Assign_Constant( T const & );
    
    /**
        Returns current value of source Attribute.
        Or returns default value (created using default constructor), if no attribute or constant value is assigned (mode Mode::Disabled).
    */
    T                               Get() const;
    
    /**
        Field can call this to modify value of source Attribute.
        Call this in reaction to user changing value of the field.
        This will issue call to Field<T>::OnChanged on this instance.
    */
    void                            Modify( T const & );
    
protected:
    /**
        Called whenever result of Field<T>::Get changes (usualy when source value changes or source changes) or when mode changes (Disabled/R/RW).
        \p real
            True when value of Field change because value of source Attribute changed.
            False when it changed because it was assigned to a different Attribute.
    */
    virtual void                    OnChanged( bool real ) = 0;
    
private:
    virtual void                    Attribute_Changed( Attribute * ) override;
    virtual void                    Attribute_Deactivated( Attribute * ) override;
    
private:
    virtual void                    AnySource_R_Impl( Attribute * ) override;
    virtual void                    AnySource_RW_Impl( Attribute * ) override;
    virtual std::type_index         Type_Impl() override;
    virtual Attribute::ValueMode    mode_Impl() const override;
    virtual Assignment              AssignmentState_impl() const override;
    
protected:
    Instance * inst;
    std::optional< T > constant;
    Attr< T > * source_r;
    Attr< T > * source_rw;
};

//====================================================================================

template<>
struct StringIO< FieldI::Assignment > : public StringIO_Table< FieldI::Assignment >
{
    static const ValuesType Values;
};

//-------------------- StringIO< Field > -------------------------------
template< class T >
struct StringIO< Field< T > >
{
    std::string Write( Field< T > const & value, Context const * context ) const
    {
        std::stringstream ss;
        ss << StringIO_Write( value.AssignmentState(), context ) << "/" << StringIO_Write( value.Mode(), context ) << "/" << StringIO_Write( value.Get(), context );
        return ss.str();
    }
};

}

#include "Field.inl"
