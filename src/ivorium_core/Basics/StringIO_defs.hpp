#pragma once

#include "StringIO.hpp"
#include "RuntimeId.hpp"
#include "Context.hpp"
#include "SS.hpp"

namespace iv
{

//------------- default definition -----------------------
template< class Type, typename Enabled >
struct StringIO
{
    Type Read( const char * source, Context const * context ) const
    {
        std::stringstream in( source );
        
        Type type;
        in >> type;
        
        // peek next character to trigger EOF flag without triggering fail flag
        if( !in.eof() )
            in.peek();
        
        if( in.fail() || !in.eof() )
        {
            std::string rest(std::istreambuf_iterator<char>(in), {});
            context->log( SRC_INFO, iv::Defs::Log::Warning, "String '", source, "' can not be read as type ", typeid( Type ).name(), " using stream operator >>. Rest: '", rest, "'." );
            return Type();
        }
        
        return type;
    }
    
    std::string Write( Type const & value, Context const * context ) const
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
};

//------ bool specialization --------------
template<>
struct StringIO< bool >
{
    bool Read( const char * source, Context const * context ) const
    {
        std::string val( source );
        return val == "true" || val == "1";
    }

    std::string Write( bool const & value, Context const * context ) const
    {
        return value ? "true" : "false";
    }
};

//---------- string specialization ----------------------
template<>
struct StringIO< std::string >
{
    std::string Read( const char * source, Context const * context ) const
    {
        return source;
    }

    std::string Write( std::string const & value, Context const * context ) const
    {
        return value;
    }
};

//---------- cstring specialization ----------------------
template<>
struct StringIO< const char * >
{
    std::string Write( const char * const & value, Context const * context ) const
    {
        return value;
    }
};

//--------- pointer specialization -------------
template< class T >
struct StringIO< T * >
{
    T * Read( const char * source, Context const * context ) const
    {
        return nullptr;
    }

    std::string Write( T * const & value, Context const * context ) const
    {
        return SS() << value << SS::str();
    }
};

//------------------- nullptr --------------------
template<>
struct StringIO< std::nullptr_t >
{
    std::nullptr_t Read( const char * source, Context const * context ) const
    {
        return nullptr;
    }

    std::string Write( std::nullptr_t const &, Context const * context ) const
    {
        return "nullptr";
    }
};

//---------------------- std::optional ----------------------------
template< class T >
struct StringIO< std::optional< T > >
{
    static constexpr const char * NIL = "nil";

    std::optional< T > Read( const char * name, Context const * context )
    {
        if( strcmp( name, NIL ) == 0 )
            return std::optional< T >( {} );
        else
            return StringIO< T >().Read( name, context );
    }

    std::string Write( std::optional< T > const & value, Context const * context ) const
    {
        if( value.has_value() )
            return StringIO< T >().Write( value.value(), context );
        else
            return std::string( NIL );
    }
};

//------------- std::pair -------------------
template< class T, class U >
struct StringIO< std::pair< T, U > >
{
    std::pair< T, U > Read( const char * name, Context const * context )
    {
        std::string first;
        std::string second;
        
        size_t i = 0;
        size_t len = strlen( name );
        while( i < len && name[ i ] != '(' )
            i++;
        i++;
        
        while( i < len && i != ',' )
            first += name[ i++ ];
        i++;
        
        while( i < len && i != ')' )
            second += name[ i++ ];
        
        T t = StringIO_Read< T >( first.c_str(), context );
        U u = StringIO_Read< U >( second.c_str(), context );
        return std::pair( t, u );
    }
    
    std::string Write( std::pair< T, U > const & value, Context const * context ) const
    {
        std::stringstream ss;
        ss << "( " << StringIO_Write< T >( value.first, context ) << ", " << StringIO_Write< U >( value.second, context ) << " )";
        return ss.str();
    }
};



//-------------- floating point values ---------------------------
template< class Float >
struct StringIO< Float, std::enable_if_t< std::is_floating_point< Float >::value > >
{
    Float Read( const char * source, Context const * context ) const
    {
        //------- special values -----------
        if( strcmp( source, "inf" ) == 0 )
            return std::numeric_limits< Float >::infinity();
        else if( strcmp( source, "+inf" ) == 0 )
            return std::numeric_limits< Float >::infinity();
        else if( strcmp( source, "-inf" ) == 0 )
            return - std::numeric_limits< Float >::infinity();
        else if( strcmp( source, "nan" ) == 0 )
            return - std::numeric_limits< Float >::quiet_NaN();
        else if( strcmp( source, "NaN" ) == 0 )
            return - std::numeric_limits< Float >::quiet_NaN();
        
        //-------- normal read ------------
        std::stringstream in( source );
        
        Float type;
        in >> type;
        
        // peek next character to trigger EOF flag without triggering fail flag
        if( !in.eof() )
            in.peek();
        
        if( in.fail() || !in.eof() )
        {
            std::string rest(std::istreambuf_iterator<char>(in), {});
            context->warning( SRC_INFO, "String '", source, "' can not be read as float using stream operator >>. Rest: '", rest, "'." );
            return Float();
        }
        
        return type;
    }

    std::string Write( Float const & value, Context const * context ) const
    {
        return std::to_string( value );
    }
};


//-------------------- RuntimeId -------------------------------
template< class TypedRuntimeId >
struct StringIO< TypedRuntimeId, std::enable_if_t< std::is_base_of< RuntimeId< TypedRuntimeId >, TypedRuntimeId >::value > >
{
    TypedRuntimeId Read( const char * source, Context const * context ) const
    {
        auto result = TypedRuntimeId( source );
        if( !result.valid() )
            context->warning( SRC_INFO, "Can not read string '", source, "' as RuntimeId ", typeid( TypedRuntimeId ).name(), "." );
        
        return result;
    };

    std::string Write( TypedRuntimeId const & value, Context const * context ) const
    {
        return value.persistent_value();
    }
};

}

#include "StringIOIndex.hpp"    // Include this just to make sure that StringIO< std::any > is defined - it is defined there and we otherwise could get into situation where both std::any and StringIO are defined but StringIO< std::any > is not.
