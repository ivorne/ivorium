#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "StringIO.hpp"
#include "SS.hpp"
#include "Interpolator.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <array>
#include <iomanip>


//-------------- float3 -----------------------
namespace iv
{

using int2 = glm::tvec2< int >;
using int3 = glm::tvec3< int >;

using float2 = glm::tvec2< float >;
using float3 = glm::tvec3< float >;
using float4 = glm::tvec4< float >;

using bool2 = glm::tvec2< bool >;
using bool3 = glm::tvec3< bool >;

using uchar = unsigned char;
using uchar4 = glm::tvec4< unsigned char >;

using float4x4 = glm::tmat4x4< float >;

using floatQuat = glm::tquat< float >;


template< typename Type >
struct StringIO< glm::tvec2< Type > >
{
    glm::tvec2< Type > Read( const char * source, Context const * context ) const
    {
        return glm::tvec2< Type >( 0, 0 );
    }

    std::string Write( glm::tvec2< Type > val, Context const * context ) const
    {
        return SS() << "(" << val.x << ", " << val.y << ")" << SS::str();
    }
};

template< typename Type >
struct StringIO< glm::tvec3< Type > >
{
    glm::tvec3< Type > Read( const char * source, Context const * context ) const
    {
        return glm::tvec3< Type >( 0, 0, 0 );
    }

    std::string Write( glm::tvec3< Type > val, Context const * context ) const
    {
        return SS() << "(" << val.x << ", " << val.y << ", " << val.z << ")" << SS::str();
    }
};

template< typename Type >
struct StringIO< glm::tvec4< Type > >
{
    glm::tvec4< Type > Read( const char * source, Context const * context ) const
    {
        return glm::tvec4< Type >( 0, 0, 0, 0 );
    }

    std::string Write( glm::tvec4< Type > val, Context const * context ) const
    {
        return SS() << "(" << val.x << ", " << val.y << ", " << val.z << ", " << val.w << ")" << SS::str();
    }
};

template<>
struct StringIO< floatQuat >
{
    floatQuat Read( const char * source, Context const * context ) const
    {
        return floatQuat( 0, 0, 0, 0 );
    }

    std::string Write( floatQuat val, Context const * context ) const
    {
        return SS() << "(" << val.w << " | " << val.x << ", " << val.y << ", " << val.z << ")" << SS::str();
    }
};

template<>
struct StringIO< float4x4 >
{
    float4x4 Read( const char * source, Context const * context ) const
    {
        return float4x4( 1 );
    }

    std::string Write( float4x4 val, Context const * context ) const
    {
        // prepare
        std::array< std::array< std::string, 4 >, 4 > parts;
        std::array< size_t, 4 > widths;
        widths.fill( 0 );
        
        for( size_t x = 0; x < 4; x++ )
            for( size_t y = 0; y < 4; y++ )
            {
                std::stringstream ss;
                ss << std::setprecision( 3 ) << val[ x ][ y ];
                parts[ y ][ x ] = ss.str();
                auto w = parts[ y ][ x ].length();
                if( widths[ x ] < w )
                    widths[ x ] = w;
            }
        
        std::stringstream out;
        
        // first line
        out << "╭   ";
        for( size_t i = 0; i < widths[ 0 ] + widths[ 1 ] + widths[ 2 ] + widths[ 3 ] + 6; i++ )
            out << " ";
        out << "   ╮" << std::endl;
        
        // middle lines
        out << "│ ( " << std::setw( widths[ 0 ] ) << parts[ 0 ][ 0 ] << ", " << std::setw( widths[ 1 ] ) << parts[ 0 ][ 1 ] << ", " << std::setw( widths[ 2 ] ) << parts[ 0 ][ 2 ] << ", " << std::setw( widths[ 3 ] ) << parts[ 0 ][ 3 ] << " ) │" << std::endl;
        out << "│ ( " << std::setw( widths[ 0 ] ) << parts[ 1 ][ 0 ] << ", " << std::setw( widths[ 1 ] ) << parts[ 1 ][ 1 ] << ", " << std::setw( widths[ 2 ] ) << parts[ 1 ][ 2 ] << ", " << std::setw( widths[ 3 ] ) << parts[ 1 ][ 3 ] << " ) │" << std::endl;
        out << "│ ( " << std::setw( widths[ 0 ] ) << parts[ 2 ][ 0 ] << ", " << std::setw( widths[ 1 ] ) << parts[ 2 ][ 1 ] << ", " << std::setw( widths[ 2 ] ) << parts[ 2 ][ 2 ] << ", " << std::setw( widths[ 3 ] ) << parts[ 2 ][ 3 ] << " ) │" << std::endl;
        out << "│ ( " << std::setw( widths[ 0 ] ) << parts[ 3 ][ 0 ] << ", " << std::setw( widths[ 1 ] ) << parts[ 3 ][ 1 ] << ", " << std::setw( widths[ 2 ] ) << parts[ 3 ][ 2 ] << ", " << std::setw( widths[ 3 ] ) << parts[ 3 ][ 3 ] << " ) │" << std::endl;
        
        // last line
        out << "╰   ";
        for( size_t i = 0; i < widths[ 0 ] + widths[ 1 ] + widths[ 2 ] + widths[ 3 ] + 6; i++ )
            out << " ";
        out << "   ╯";
        
        return out.str();
    }
};



//------------------------------------- interpolator -----------------------------------------------------
template<>
struct Interpolator< float3 >
{
    Interpolator_float Distance( float3 const & from, float3 const & to ) const
    {
        return glm::distance( from, to );
    }
    
    std::pair< float3, Interpolator_float > Interpolate( float3 const & from, float3 const & to, Interpolator_float distance ) const
    {
        if( distance == std::numeric_limits< Interpolator_float >::infinity() )
            return std::pair( to, Interpolator_float( 0.0 ) );
        
        if( glm::distance( to, from ) <= distance )
            return std::pair( to, Interpolator_float( 0.0 ) );
        
        return std::pair( from + glm::normalize( to - from ) * distance, Interpolator_float( 0.0 ) );
    }
};



template<>
struct Interpolator< float4x4 >
{
    Interpolator_float Distance( float4x4 const & from, float4x4 const & to ) const
    {
        float4x4 diff( to - from );
        float distance_sq = glm::length2( glm::row( diff, 0 ) ) + glm::length2( glm::row( diff, 1 ) ) + glm::length2( glm::row( diff, 2 ) ) + glm::length2( glm::row( diff, 3 ) );
        return sqrt( distance_sq );
    }
    
    std::pair< float4x4, Interpolator_float > Interpolate( float4x4 const & from, float4x4 const & to, Interpolator_float distance ) const
    {
        float4x4 diff( to - from );
        float current_distance_sq = glm::length2( glm::row( diff, 0 ) ) + glm::length2( glm::row( diff, 1 ) ) + glm::length2( glm::row( diff, 2 ) ) + glm::length2( glm::row( diff, 3 ) );
        float current_distance = sqrt( current_distance_sq );
        
        float4x4 result;
        
        result[ 0 ][ 0 ] = from[ 0 ][ 0 ] + diff[ 0 ][ 0 ] / current_distance * distance;
        result[ 0 ][ 1 ] = from[ 0 ][ 1 ] + diff[ 0 ][ 1 ] / current_distance * distance;
        result[ 0 ][ 2 ] = from[ 0 ][ 2 ] + diff[ 0 ][ 2 ] / current_distance * distance;
        result[ 0 ][ 3 ] = from[ 0 ][ 3 ] + diff[ 0 ][ 3 ] / current_distance * distance;
        
        result[ 1 ][ 0 ] = from[ 1 ][ 0 ] + diff[ 1 ][ 0 ] / current_distance * distance;
        result[ 1 ][ 1 ] = from[ 1 ][ 1 ] + diff[ 1 ][ 1 ] / current_distance * distance;
        result[ 1 ][ 2 ] = from[ 1 ][ 2 ] + diff[ 1 ][ 2 ] / current_distance * distance;
        result[ 1 ][ 3 ] = from[ 1 ][ 3 ] + diff[ 1 ][ 3 ] / current_distance * distance;
        
        result[ 2 ][ 0 ] = from[ 2 ][ 0 ] + diff[ 2 ][ 0 ] / current_distance * distance;
        result[ 2 ][ 1 ] = from[ 2 ][ 1 ] + diff[ 2 ][ 1 ] / current_distance * distance;
        result[ 2 ][ 2 ] = from[ 2 ][ 2 ] + diff[ 2 ][ 2 ] / current_distance * distance;
        result[ 2 ][ 3 ] = from[ 2 ][ 3 ] + diff[ 2 ][ 3 ] / current_distance * distance;
        
        result[ 3 ][ 0 ] = from[ 3 ][ 0 ] + diff[ 3 ][ 0 ] / current_distance * distance;
        result[ 3 ][ 1 ] = from[ 3 ][ 1 ] + diff[ 3 ][ 1 ] / current_distance * distance;
        result[ 3 ][ 2 ] = from[ 3 ][ 2 ] + diff[ 3 ][ 2 ] / current_distance * distance;
        result[ 3 ][ 3 ] = from[ 3 ][ 3 ] + diff[ 3 ][ 3 ] / current_distance * distance;
        
        return std::pair( result, Interpolator_float( 0.0 ) );
    }
};

struct int2_CompareByX
{
    bool operator()( int2 const & left, int2 const & right ) const
    {
        return std::pair( left.x, left.y ) < std::pair( right.x, right.y );
    }
};

struct int2_CompareByY
{
    bool operator()( int2 const & left, int2 const & right ) const
    {
        return std::pair( left.y, left.x ) < std::pair( right.y, right.x );
    }
};

}

inline std::ostream & operator<<( std::ostream & out, iv::int2 const & val )
{
    out << "( " << val.x << ", " << val.y << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::int3 const & val )
{
    out << "( " << val.x << ", " << val.y << ", " << val.z << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::float2 const & val )
{
    out << "( " << val.x << ", " << val.y << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::float3 const & val )
{
    out << "( " << val.x << ", " << val.y << ", " << val.z << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::float4 const & val )
{
    out << "( " << val.x << ", " << val.y << ", " << val.z << ", " << val.w << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::bool2 const & val )
{
    out << "( " << val.x << ", " << val.y << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::bool3 const & val )
{
    out << "( " << val.x << ", " << val.y << ", " << val.z << " )";
    return out;
}

inline std::ostream & operator<<( std::ostream & out, iv::uchar4 const & val )
{
    out << "( " << val.x << ", " << val.y << ", " << val.z << ", " << val.w << " )";
    return out;
}

