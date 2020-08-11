#pragma once

#include "../Animation/AnimConnector.hpp"
#include <functional>

namespace iv
{

/**
*/
class TimeTransform : public std::function< Anim_float( Anim_float ) >
{
public:
    using std::function< Anim_float( Anim_float ) >::function;
    
    Anim_float Transform( Anim_float src_time, Anim_float total_time ) const
    {
        // do we even use a transformation?
        if( !(*this) )
            return src_time;
        
        // use linear transform if src_time is out of bounds
        if( src_time > total_time || src_time < Anim_float( 0.0 ) )
            return src_time;
        
        // get source factor
        Anim_float src_factor = src_time / total_time;
        
        // user transform
        Anim_float tgt_factor = (*this)( src_factor );
        tgt_factor = iv::clamp( tgt_factor, Anim_float( 0.0 ), Anim_float( 1.0 ) );
        
        // target time
        Anim_float tgt_time = tgt_factor * total_time;
        return tgt_time;
    }
};

/**
    No transformation.
*/
struct LinearTransform
{
    Anim_float operator()( Anim_float x )
    {
        return x;
    }
};

/**
    Sigmoid transformation.
    https://en.wikipedia.org/wiki/Smoothstep
*/
struct Smoothstep2Transform
{
public:
    Anim_float operator()( Anim_float x )
    {
        return x*x*( 3.0 - 2.0*x ) ;
    }
};

/**
    Sigmoid transformation (more curvy than Smoothstep2Transform).
    https://en.wikipedia.org/wiki/Smoothstep
*/
struct Smoothstep3Transform
{
public:
    Anim_float operator()( Anim_float x )
    {
        return x*x*x*( x*( x*6.0 - 15.0 ) + 10.0 );
    }
};

/**
    Fast at the beginning, slowing at the end.
*/
struct QuickstepTransform
{
public:
    Anim_float operator()( Anim_float x )
    {
        return 1.0 - ( 1.0-x )*( 1.0-x );
    }
};

/**
    When \p degree is:
        * 1 - Linear transform.
        * <1 - Faster at the beginning, slower at the end.
        * >1 - Slower at the beginning, faster at the end.
        * <=0 - Invalid.
*/
struct ExponentialTransform
{
public:
    ExponentialTransform( Anim_float degree ) : degree( degree ){}

    Anim_float operator()( Anim_float x )
    {
        if( degree == Anim_float( 1.0 ) )
            return x;
        else
            return ( pow( this->degree, x ) - Anim_float( 1.0 ) ) / ( this->degree - Anim_float( 1.0 ) );
    }
    
private:
    Anim_float degree;
};

}
