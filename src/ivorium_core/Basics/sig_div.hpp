#pragma once

#include "glm_alias.hpp"

namespace iv
{

inline int sig_div( int a, unsigned b )
{
    if( a >= 0 )
        return a / (int)b;
    else
        return ( a - int( b ) + 1 ) / (int)b;
}

inline int2 sig_div( int2 a, unsigned b )
{
    return int2( sig_div( a.x, b ), sig_div( a.y, b ) );
}

inline int sig_mul( int a, unsigned b )
{
    return a * b;
}

inline int2 sig_mul( int2 a, unsigned b )
{
    return int2( sig_mul( a.x, b ), sig_mul( a.y, b ) );
}

inline int sig_mod( int a, unsigned b )
{
    if( a >= 0 )
        return a % (int)b;
    else
        return ( a + 1 ) % int( b ) + int( b ) - 1;
}

inline int2 sig_mod( int2 a, unsigned b )
{
    return int2( sig_mod( a.x, b ), sig_mod( a.y, b ) );
}

inline float sig_fmod( float val, float mod )
{
    if( ( val < 0.0f ) == ( mod < 0.0f ) )
        return val - mod * float( int( val / mod ) );
    else
        return val - mod * float( int( val / mod ) - 1 );
}

}
