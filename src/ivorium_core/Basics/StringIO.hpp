#pragma once

#include <initializer_list>
#include <cstring>
#include <string>
#include <optional>
#include <limits>

namespace iv
{

class Context;

//------------ declaration -----------------------
template<typename _Tp, typename Enabled=void>
struct StringIO;

//--------
template< class Type >
Type StringIO_Read( const char * source, Context const * context )
{
    return StringIO< Type >().Read( source, context );
}

inline std::string StringIO_Write( char const * const & value, Context const * context )
{
    return value;
}

template< class Type >
std::string StringIO_Write( Type const & value, Context const * context )
{
    return StringIO< Type >().Write( value, context );
}

}
