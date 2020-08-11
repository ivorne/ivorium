#include "FunctorActivatorAttribute.hpp"

namespace iv
{

FunctorActivatorAttribute::FunctorActivatorAttribute( ClientMarker * cm, std::function< void() > const & on_activated ) :
    Local_AEP(),
    Attr< Activator >( cm, this, false ),
    activator(),
    _on_activated( on_activated )
{
    this->SetAttributeMode( Attribute::ValueMode::ValueModify );
    this->Notify_Activated();
}

FunctorActivatorAttribute::~FunctorActivatorAttribute()
{
    this->Notify_Deactivated();
}

void FunctorActivatorAttribute::on_activated( std::function< void() > const & val )
{
    this->_on_activated = val;
}

void FunctorActivatorAttribute::SetAttributeMode( Attribute::ValueMode mode )
{
    this->Attr< Activator >::SetAttributeMode( mode );
}

void FunctorActivatorAttribute::GetSourceValue( Activator & out ) const
{
    out = this->activator;
}

void FunctorActivatorAttribute::ModifySource( Activator const & val )
{
    if( val.CopyActivations( this->activator ) && this->_on_activated )
        this->_on_activated();
    
    this->Attr< Activator >::SourceValueChanged();
}

}
