#pragma once

#include "../Instancing/Instance.hpp"
#include "../Basics/volatile_set.hpp"
#include "../Basics/StringIO_Table.hpp"
#include "../Basics/valgrind.hpp"

#include <typeindex>
#include <any>

namespace iv
{

class Attribute;

//==================== AttributeListener ==========================
class AttributeListener
{
public:
    /**
        No usually used. Might be used by some kind of implicit listeners.
    */
    virtual void            Attribute_Activated( Attribute * ){}
    
    /**
        Called during destruction of the attribute.
        There is no need to unregister the listener using Attribute::AttributeListener at that point.
    */
    virtual void            Attribute_Deactivated( Attribute * ){}
    
    /**
        Called whenever the attribute (source) changes its value and this should be propagated to its listener.
    */
    virtual void            Attribute_Changed( Attribute * ){}
};

//==================== AttributeEventProcessor ========================
class AttributeEventProcessor
{
public:
    enum class Event
    {
        Activated,
        Deactivated,
        Changed,
        AddListener,
        RemoveListener
    };
    
    /**
        Parameter \p listener is relevant only for AddListener and RemoveListener events.
    */
    virtual void AttributeEvent( AttributeEventProcessor::Event event, Attribute * attr, AttributeListener * listener ) = 0;
};

//====================== Attribute ================================
class Attribute
{
public:
    enum class ValueMode
    {
        Disabled,       ///< No value is present. Connected fields are disabled, input fields are greyed out.
        Value,          ///< Has a read only value assigned.
        ValueModify     ///< Has a readable and modifiable value.
    };
    
                                /**
                                    \p event_processor This needs to be valid and preferably persistent across the whole life-cycle of the Attribute.
                                    \p allow_ModifyMode Set this to false when mode should be modifiable only through protected interface, set this to true and it can also be modified through public interface using Attribute::ModifyMode method. When set to true, Attribute::Modify will also change mode to ValueModify automatically.
                                */
                                Attribute( ClientMarker *, AttributeEventProcessor * event_processor, bool allow_ModifyMode );
    
    /**
    */
    ClientMarker *              owner() const;
    
    /**
    */
    std::type_index             Type();
    
    /**
    */
    ValueMode                   Mode() const;
    
    /**
    */
    std::any                    GetAny();
    
    /**
        When Attribute acts as interface between Behavior and View, then it is considered best practice to set mode from Behavior, rather than View.
    */
    void                        ModifyMode( ClientMarker * modifier, Attribute::ValueMode );
    
    /**
        If the attribute is not writable, this will have no effect (without warning).
        It is good practise to not call this when not knowing the type and modifiability of the field.
    */
    void                        ModifyAny( ClientMarker * modifier, std::any );
    
    /**
    */
    void                        Insert_Listener( AttributeListener * );
    
    /**
    */
    void                        Remove_Listener( AttributeListener * );
    
protected:
    void                        SetAttributeMode( ValueMode );
    void                        SourceValueChanged();
    
    AttributeEventProcessor *   event_processor();
    void                        event_processor( AttributeEventProcessor * );
    
    /**
        Should be called when Attr is properly connected to a AttributeEventProcessor capable of processing the events.
    */
    void                        Notify_Activated();
    
    /**
        Should be called before disconnecting Attr from its AttributeEventProcessor, losing the capability to process events.
    */
    void                        Notify_Deactivated();
    
    virtual std::type_index     Type_Impl() = 0;
    virtual std::any            GetAny_Impl() = 0;
    virtual void                ModifyAny_Impl( ClientMarker * modifier, std::any ) = 0;
    
    bool                        allow_ModifyMode();
    
private:
    ClientMarker * cm;
    ValueMode _mode;
    AttributeEventProcessor * _processor;
    bool _allow_ModifyMode;
};

//===================== Attr ============================
/**
*/
template< class T >
class Attr : public Attribute
{
public:
                                Attr( ClientMarker *, AttributeEventProcessor *, bool allow_ModifyMode );
    
    T                           Get() const;
    
    /**
    */
    void                        Modify( ClientMarker * modifier, T const & );
    
protected:
    /**
    */
    virtual void                GetSourceValue( T & out ) const = 0;
    
    /**
        This should call Attr< T >::SourceValueChanged after the value is changed (can be skipped if value is the same or if the changes were not applied for some reason).
    */
    virtual void                ModifySource( T const & ) = 0;
    
private:
    virtual std::type_index     Type_Impl() override;
    virtual std::any            GetAny_Impl() override;
    virtual void                ModifyAny_Impl( ClientMarker * modifier, std::any ) override;
};


//=================== StringIO ========================================================================
template<>
struct StringIO< Attribute::ValueMode > : public StringIO_Table< Attribute::ValueMode >
{
    static const ValuesType Values;
};

template<>
struct StringIO< AttributeEventProcessor::Event > : public StringIO_Table< AttributeEventProcessor::Event >
{
    static const ValuesType Values;
};

template< class T >
struct StringIO< Attr< T > >
{
    std::string Write( Attr< T > const & value, Context const * context ) const
    {
        std::stringstream ss;
        ss << StringIO_Write( value.Mode(), context ) << "/" << StringIO_Write( value.Get(), context );
        return ss.str();
    }
};

}

#include "Attribute.inl"
