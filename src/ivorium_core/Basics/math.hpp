#pragma once

namespace iv
{

template< class Float >
Float mix_max( Float from, Float to, Float maxDist, std::enable_if_t< std::is_floating_point< Float >::value, std::nullptr_t > = nullptr )
{
    Float result;
    if( to > from )
    {
        result = from + maxDist;
        if( result > to )
            result = to;
    }
    else
    {
        result = from - maxDist;
        if( result < to )
            result = to;
    }
    
    return result;
}

template< class Float >
Float abs( Float val )
{
    return ( val > 0 ) ? ( val ) : ( Float( 0 ) - val );
}

template< class Float >
Float clamp( Float val, Float min, Float max )
{
    return std::max( min, std::min( max, val ) );
}

//------------ math --------------------
/*
template< class P, class Type >
inline Type lerp( double factor, Vector< P, Type, 2 > const & bounds )
{
    return bounds.template get< 0 >() + factor * ( bounds.template get< 1 >() - bounds.template get< 0 >() );
}

template< class Type >
inline Type lerp( double factor, Type const & bound_min, Type const & bound_max )
{
    return bound_min + factor * ( bound_max - bound_min );
}



template< class P, class Type >
inline double compute_factor( Type value, Vector< P, Type, 2 > const & bounds )
{
    return double( value -  bounds.template get< 0 >() ) / double( bounds.template get< 1 >() - bounds.template get< 0 >() );
}

template< class Type >
inline double compute_factor( Type value, Type bound_min, Type bound_max )
{
    return double( value -  bound_min ) / double( bound_max - bound_min );
}


template< class Float >
inline Float angle_normalize( Float angle )
{
    if( angle > Float( 2.0*math_pi ) )
    {
        double divisor = angle / Float( 2.0*math_pi );
        return angle - int( divisor ) * Float( 2.0*math_pi );
    }
    else if( angle < Float( 0.0 ) )
    {
        double divisor = angle / Float( -2.0*math_pi );
        angle = angle + ( int( divisor )+1 ) * Float( 2.0*math_pi );
        if( angle == Float( 2.0*math_pi ) )
            return Float( 0.0 );
        else
            return angle;
    }
    else
    {
        return angle;
    }
}



/// Computes value, that has the same factor in \p new_range as \p original_value has in \p original_range.
/// \returns new_value
template< class _P, class Float >
inline Float linear_remap( Float const & original_value, Vector< _P, Float, 2 > const & original_range, Vector< _P, Float, 2 > const & new_range  )
{
    auto factor = compute_factor( original_value, original_range );
    return lerp( factor, new_range );
}

*/

}
