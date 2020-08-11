#pragma once

#include <sstream>
#include <string>
#include <iostream>

namespace iv
{

class SS
{
public:

                            SS();
                            SS( char const * val );
    
// stream operations
    struct str{};
    std::string             operator<<( str );
    std::string             operator<<( str ) const;
    
    struct c_str{};
    char const *            operator<<( c_str );
    char const *            operator<<( c_str ) const;
   
    template< class Type >
    SS &                    operator<<( Type const & value );
    
private:
  std::ostringstream    oss;
  mutable std::string   tmp;
};

template< class Type >
SS & SS::operator<<( Type const & value )
{
  ((std::ostream&)this->oss) << value;
  return *this;
}

}
