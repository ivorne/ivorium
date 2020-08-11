#pragma once

#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <optional>
#include <vector>

namespace iv
{

//------------------------- iterators --------------------------------
template< class T >
struct reversed
{
public:
    reversed( T & cont ) : cont( cont ){}
    
    typename T::reverse_iterator begin(){ return this->cont.rbegin(); }
    typename T::reverse_iterator end(){ return this->cont.rend(); }
    
private:
    T & cont;
};

///**
//  std::unordered_map< int, Item * > m;
//  Item * item = at_or( m, 13, nullptr );
//  if( item )
//  {
//      ...
//  }
//*/
//template< class Key, class T, class Hash, class KeyEqual, class Allocator >
//T const & at_or( std::unordered_map< Key, T, Hash, KeyEqual, Allocator > const & cont, Key const & key, T const & def )
//{
//  auto it = cont.find( key );
//  if( it == cont.end() )
//      return def;
//  else
//      return it->second;
//}

//----------------- utf8 -----------------------------------
size_t  utf8_size( const char * );
size_t  utf8_size( std::string const & );
bool    utf8_is_first_byte( char ); ///< If char is a last byte in an utf8 character.

//------------------ string --------------------------------
void            string_explode( std::string const & s, char delim, std::vector< std::string > & result );
std::string     string_trim( std::string const & s );
std::string     string_ltrim( std::string const & s );
std::string     string_rtrim( std::string const & s );

}
