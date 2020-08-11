#include "SS.hpp"

namespace iv
{

SS::SS()
{
}

SS::SS( char const * val )
{
    this->oss << val;
}

std::string SS::operator<<( str )
{
    return this->oss.str();
}

std::string SS::operator<<( str ) const
{
    return this->oss.str();
}

char const * SS::operator<<( c_str )
{
    this->tmp = this->oss.str();
    return this->tmp.c_str();
}

char const * SS::operator<<( c_str ) const
{
    this->tmp = this->oss.str();
    return this->tmp.c_str();
}

}
