#include "Activator.hpp"

namespace iv
{

Activator::Activator() :
    _value( 0 )
{
}

Activator::Activator( unsigned char initial ) :
    _value( initial )
{
}

bool Activator::operator==( Activator const & other ) const
{
    return this->_value == other._value;
}

bool Activator::operator!=( Activator const & other ) const
{
    return this->_value != other._value;
}

bool Activator::operator<( Activator const & other ) const
{
    return std::tie( this->_value ) < std::tie( other._value );
}

unsigned char Activator::ReadActivations( Activator const & reference ) const
{
    unsigned top = this->_value;
    unsigned bot = reference._value;
    if( top < bot )
        top += 256;
    return top - bot;
}

unsigned char Activator::CopyActivations( Activator & reference ) const
{
    auto act = this->ReadActivations( reference );
    reference._value = this->_value;
    return act;
}

bool Activator::CopyActivation( Activator & reference ) const
{
    if( this->_value == reference._value )
        return false;
    reference._value++;
    return true;
}

Activator Activator::MakeActivated() const
{
    Activator result = *this;
    result._value++;
    return result;
}

unsigned char Activator::Value() const
{
    return this->_value;
}

}
