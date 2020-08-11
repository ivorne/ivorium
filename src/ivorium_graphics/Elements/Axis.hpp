#pragma once

#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

enum class Axis
{
    X,
    Y,
    Z,
    _Size
};

enum class AxisOrder
{
    Incremental,
    Decremental,
};

enum class AxisDirection
{
    X_Inc,
    X_Dec,
    Y_Inc,
    Y_Dec,
    Z_Inc,
    Z_Dec,
};


template<>
struct StringIO< Axis > : public StringIO_Table< Axis >
{
    static const ValuesType Values;
};

template<>
struct StringIO< AxisOrder > : public StringIO_Table< AxisOrder >
{
    static const ValuesType Values;
};

template<>
struct StringIO< AxisDirection > : public StringIO_Table< AxisDirection >
{
    static const ValuesType Values;
};


inline float & float3_Axis( float3 & vec, Axis axis )
{
    if( axis == Axis::X )
        return vec.x;
    else if( axis == Axis::Y )
        return vec.y;
    else
        return vec.z;
}

inline float const & float3_Axis( float3 const & vec, Axis axis )
{
    if( axis == Axis::X )
        return vec.x;
    else if( axis == Axis::Y )
        return vec.y;
    else
        return vec.z;
}

template< class Callable >
inline void foreach_axis( Callable c )
{
    c( Axis::X );
    c( Axis::Y );
    c( Axis::Z );
}

}
