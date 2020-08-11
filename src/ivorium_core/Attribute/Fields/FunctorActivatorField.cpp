#include "FunctorActivatorField.hpp"

namespace iv
{

FunctorActivatorField::FunctorActivatorField( Instance * inst, std::function< void() > const & on_activated ) :
    Field< Activator >( inst ),
    cm( inst, this, "FunctorActivatorField" ),
    _on_activated( on_activated )
{
    this->cm.inherits( this->Field< Activator >::cm );
}

void FunctorActivatorField::on_activated( std::function< void() > const & f )
{
    this->_on_activated = f;
}

void FunctorActivatorField::OnChanged( bool real )
{
    if( this->Get().CopyActivations( this->_reference ) && real && this->_on_activated )
        this->_on_activated();
}

}
