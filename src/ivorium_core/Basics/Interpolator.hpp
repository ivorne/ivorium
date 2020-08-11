#pragma once

#include "glm_alias.hpp"
#include "math.hpp"
#include "utils.hpp"

#include <type_traits>

namespace iv
{

namespace interpolator
{

template< typename T >
struct to_void
{
    typedef void type;
};

template< typename T >
struct always_false : std::false_type{};

}


using Interpolator_float = float;

template< typename T, typename dummy = void >
struct has_interpolate : std::false_type{};

template< typename T >
struct has_interpolate< T, typename interpolator::to_void<typename T::interpolate>::type > : std::true_type{};

template< typename Type, typename Enabled = void >
struct Interpolator
{
    /**
        Returns distance when traveling from value \p from to value \p to.
        As distance, it is expected to be positive but I will not say that negative values can't make sense in any context.
    */
    Interpolator_float Distance( Type const & from, Type const & to ) const
    {
        static_assert( interpolator::always_false< Type >::value, "Interpolator::Distance not defined for given type." );
        return 0.0f;
    }
    
    /**
        Interpolates between values \p from and \p to. 
        This will never go outside of this range (so infinite distance should return \p to, not infinite value).
        Parameter \p distance is distance corresponding to value returned from Interpolator::Distance, it is not mixing factor as in Mix functions (that use values from 0.0 to 1.0).
        \returns
            First value is the resulting interpolated value.
            Second value is remainder - when interpolating integer values, we might not need whole distance to travel to the resulting value.
                So the remainder is computed, stored for future used and in next interpolation (if the target is the same) will be added to new interpolation distance.
                This allows us to animate integer values even with low speed and small animation steps.
    */
    std::pair< Type, Interpolator_float > Interpolate( Type const & from, Type const & to, Interpolator_float distance ) const
    {
        static_assert( interpolator::always_false< Type >::value, "Interpolator::Interpolate not defined for given type." );
    }
};


//---------- optional -------------
template< class T >
struct Interpolator< std::optional< T > >
{
    Interpolator_float Distance( std::optional< T > const & from, std::optional< T > const & to ) const
    {
        if( from.has_value() && to.has_value() )
        {
            return Interpolator< T >().Distance( from.value(), to.value() );
        }
        else
        {
            return 0;
        }
    }
    
    std::pair< std::optional< T >, Interpolator_float > Interpolate( std::optional< T > const & from, std::optional< T > const & to, Interpolator_float distance ) const
    {
        if( from.has_value() && to.has_value() )
        {
            return Interpolator< T >().Interpolate( from.value(), to.value(), distance );
        }
        else
        {
            return std::pair( to, 0 );
        }
    }
};

//---------- float -------------
template< class Float >
struct Interpolator< Float, std::enable_if_t< std::is_floating_point< Float >::value > >
{
    Interpolator_float Distance( Float const & from, Float const & to ) const
    {
        return iv::abs( to - from );
    }
    
    std::pair< Float, Interpolator_float > Interpolate( Float const & from, Float const & to, Interpolator_float distance ) const
    {
        if( distance == std::numeric_limits< Interpolator_float >::infinity() )
            return std::pair( to, 0.0f );
        
        return std::pair( mix_max( from, to, Float( distance ) ), 0.0f );
    }
};

//---------- int -------------
template< class Int >
struct Interpolator< Int, std::enable_if_t< std::is_integral< Int >::value > >
{
    Interpolator_float Distance( Int const & from, Int const & to ) const
    {
        return iv::abs( to - from );
    }
    
    std::pair< Int, Interpolator_float > Interpolate( Int const & from, Int const & to, Interpolator_float distance ) const
    {
        if( distance == std::numeric_limits< Interpolator_float >::infinity() )
            return std::pair( to, 0.0f );
        
        Interpolator_float fresult = mix_max( Interpolator_float( from ), Interpolator_float( to ), distance );
        Int result = Int( fresult );
        return std::pair( result, iv::abs( fresult - result ) );
    }
};


//#error "ASDF"

//-------------- std::string -----------------------
template<>
struct Interpolator< std::string >
{
    Interpolator_float Distance( std::string const & from, std::string const & to ) const
    {
        return 1.0;
    }
    
    std::pair< std::string, Interpolator_float > Interpolate( std::string const & from, std::string const & to, Interpolator_float distance ) const
    {
        if( distance < 1.0 )
            return std::pair( from, distance );
        else
            return std::pair( to, distance - 1.0 );
    }
};

}
