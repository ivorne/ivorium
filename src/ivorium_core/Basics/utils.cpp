#include "utils.hpp"

#include <sstream>

namespace iv
{

size_t utf8_size( std::string const & val )
{
    return utf8_size( val.c_str() );
}

size_t utf8_size( const char * val )
{
    size_t res = 0;
    for( size_t i = 0; val[ i ]; i++ )
    {
        char c = val[ i ];
        if( ( c & 0b11000000 ) == 0b10000000 )  // non-first bytes in utf8 begin with 0b10
            continue;
        res++;
    }
    
    return res;
}

bool utf8_is_first_byte( char c )
{
    return ( c & 0b11000000 ) != 0b10000000;
}

void string_explode( std::string const & s, char delim, std::vector< std::string > & result )
{
    std::istringstream iss(s);
    for (std::string token; std::getline(iss, token, delim); )
        result.push_back(std::move(token));
    if( result.empty() )
        result.push_back( "" );
}


std::string string_trim( std::string const & s )
{
    int left = -1;
    int right = 0;
    
    for( char c : s )
        if( c==' ' || c=='\n' || c=='\r' || c=='\t' )
            right++;
        else if( left<0 )
            left = right,
            right = 0;
        else
            right = 0;
    
    if( left<0 )
        return std::string("");
    else
        return s.substr( left, s.size()-right-left );
}

std::string string_ltrim( std::string const & s )
{
    int left = -1;
    int tmp = 0;
    
    for( char c : s )
        if( c==' ' || c=='\n' || c=='\r' || c=='\t' )
            tmp++;
        else if( left<0 )
            left = tmp;
    
    if( left<0 )
        return std::string("");
    else
        return s.substr( left, s.size()-left );
}

std::string string_rtrim( std::string const & s )
{
    int right = 0;
    
    for( char c : s )
        if( c==' ' || c=='\n' || c=='\r' || c=='\t' )
            right++;
        else
            right = 0;
    
    if( size_t(right) == s.size() )
        return std::string("");
    else
        return s.substr( 0, s.size()-right );
}

}
