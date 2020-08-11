#pragma once

#include "glm_alias.hpp"
#include <tuple>

namespace iv
{

//----------- iv::hash -------------------------
template<typename _Tp, typename Enabled=void>
struct hash;


//------------- use std::hash by default -----------------------
template< class Type, class Enabled >
struct hash
{
    typedef size_t result_type;
    typedef Type argument_type;
    
    size_t operator()( Type const & value ) const
    {
        return std::hash< Type >()( value );
    }
};

//-------------- enum hash ------------------------------------------
template< class Enum >
struct hash< Enum, typename std::enable_if< std::is_enum< Enum >::value >::type >
{
    size_t operator()( Enum const & value ) const
    {
        using Underlying = typename std::underlying_type< Enum >::type;
        return std::hash< Underlying >()( static_cast< Underlying >( value ) );
    }
};

//-------------- tuple and pair hash --------------------------------
namespace priv
{
    template <class T>
    inline void hash_combine(std::size_t& seed, T const& v)
    {
        seed ^= iv::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    template <class Tuple, size_t Index = ::std::tuple_size<Tuple>::value - 1>
    struct HashValueImpl
    {
      static void apply(size_t& seed, Tuple const& tuple)
      {
        HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
        hash_combine(seed, ::std::get<Index>(tuple));
      }
    };

    template <class Tuple>
    struct HashValueImpl<Tuple,0>
    {
      static void apply(size_t& seed, Tuple const& tuple)
      {
        hash_combine(seed, ::std::get<0>(tuple));
      }
    };
}

template< typename... TupleTypes >
struct hash< std::tuple< TupleTypes... > >
{
    size_t operator()( std::tuple< TupleTypes... > const & value ) const
    {
        size_t seed = 0;                             
        priv::HashValueImpl< std::tuple< TupleTypes... > >::apply( seed, value );    
        return seed;    
    }
};

template< typename First, typename Second >
struct hash< std::pair< First, Second > >
{
    size_t operator()( std::pair< First, Second > const & value ) const
    {
        return ::iv::hash< std::tuple< First, Second > >()( std::make_tuple( value.first, value.second ) );
    }
};

//--------------------- glm ---------------------------------------
template<>
struct hash< iv::int2 >
{
    size_t operator()( iv::int2 const & val ) const
    {
        size_t seed = 0;                             
        iv::priv::hash_combine( seed, val.x );
        iv::priv::hash_combine( seed, val.y );
        return seed;    
    }
};

template<>
struct hash< iv::int3 >
{
    size_t operator()( iv::int3 const & val ) const
    {
        size_t seed = 0;                             
        iv::priv::hash_combine( seed, val.x );
        iv::priv::hash_combine( seed, val.y );
        iv::priv::hash_combine( seed, val.z );
        return seed;    
    }
};

}
