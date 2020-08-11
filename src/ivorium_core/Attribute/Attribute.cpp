#include "Attribute.hpp"

namespace iv
{

const StringIO< Attribute::ValueMode >::ValuesType StringIO< Attribute::ValueMode >::Values = 
{
    { Attribute::ValueMode::Disabled, "Disabled" },
    { Attribute::ValueMode::Value, "Value" },
    { Attribute::ValueMode::ValueModify, "ValueModify" }
};

const StringIO< AttributeEventProcessor::Event >::ValuesType StringIO< AttributeEventProcessor::Event >::Values = 
{
    { AttributeEventProcessor::Event::Activated, "Activated" },
    { AttributeEventProcessor::Event::Deactivated, "Deactivated" },
    { AttributeEventProcessor::Event::Changed, "Changed" },
    { AttributeEventProcessor::Event::AddListener, "AddListener" },
    { AttributeEventProcessor::Event::RemoveListener, "RemoveListener" }
};

Attribute::Attribute( ClientMarker * cm, AttributeEventProcessor * processor, bool allow_ModifyMode ) :
    cm( cm ),
    _mode( Attribute::ValueMode::Disabled ),
    _processor( processor ),
    _allow_ModifyMode( allow_ModifyMode )
{
}

ClientMarker * Attribute::owner() const
{
    return this->cm;
}

void Attribute::Insert_Listener( AttributeListener * listener )
{
    this->_processor->AttributeEvent( AttributeEventProcessor::Event::AddListener, this, listener );
}

void Attribute::Remove_Listener( AttributeListener * listener )
{
    this->_processor->AttributeEvent( AttributeEventProcessor::Event::RemoveListener, this, listener );
}

std::type_index Attribute::Type()
{
    return this->Type_Impl();
}

std::any Attribute::GetAny()
{
    return this->GetAny_Impl();
}

void Attribute::ModifyMode( ClientMarker * modifier, Attribute::ValueMode mode )
{
    if( !this->_allow_ModifyMode )
    {
        this->owner()->warning( SRC_INFO, "Can not modify Attribute mode - it is not allowed by the owner of this Attribute to modify its mode with public Attribute::ModifyMode method." );
        return;
    }
    
    this->SetAttributeMode( mode );
    this->SourceValueChanged();
}

void Attribute::ModifyAny( ClientMarker * modifier, std::any val )
{
    this->ModifyAny_Impl( modifier, val );
}

Attribute::ValueMode Attribute::Mode() const
{
    return this->_mode;
}

void Attribute::SetAttributeMode( ValueMode val )
{
    this->_mode = val;
    this->SourceValueChanged();
}

void Attribute::Notify_Activated()
{
    if( this->_processor )
        this->_processor->AttributeEvent( AttributeEventProcessor::Event::Activated, this, nullptr );
}

void Attribute::Notify_Deactivated()
{
    if( this->_processor )
        this->_processor->AttributeEvent( AttributeEventProcessor::Event::Deactivated, this, nullptr );
}

void Attribute::SourceValueChanged()
{
    if( this->_processor )
        this->_processor->AttributeEvent( AttributeEventProcessor::Event::Changed, this, nullptr );
}

AttributeEventProcessor * Attribute::event_processor()
{
    return this->_processor;
}

void Attribute::event_processor( AttributeEventProcessor * processor )
{
    this->_processor = processor;
}

bool Attribute::allow_ModifyMode()
{
    return this->_allow_ModifyMode;
}

}
