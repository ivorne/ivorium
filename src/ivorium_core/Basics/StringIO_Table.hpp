#pragma once

#include "StringIO.hpp"
#include "SS.hpp"
#include "Context.hpp"

namespace iv
{

//-------- table (eg enum) helper -----------------
/**
Example usage:
    // enum definition
    enum Asdf
    {
      A0, A1, A2
    };
    
    // in header
    namespace iv
    {
    template<>
    struct StringIO< Asdf > : public TableStringIO< Asdf >
    {
        static const ValuesType Values;
    };
    }
    
    // in cpp
    namespace iv
    {
    const StringIO< Asdf >::ValuesType StringIO< Asdf >::Values = 
    {
        { Asdf::A0, "A0" },
        { Asdf::A1, "A1" },
        { Asdf::A2, "A2" }
    };
    }
*/
template< class Enum >
struct StringIO_Table
{
    using ValuesType = std::initializer_list< std::pair< Enum, const char * > >;
    
    Enum Read( const char * source, Context const * context ) const
    {
        for( std::pair< Enum, const char * > const & value : StringIO< Enum >::Values )
        {
            if( strcmp( source, value.second ) == 0 )
                return value.first;
        }
        
        SS ss;
        ss<<"Can not read string '"<<source<<"' as type "<<typeid( Enum ).name()<<".\n";
        ss<<"   possible values: ";
        for( std::pair< Enum, const char * > const & value : StringIO< Enum >::Values )
            ss << value.second << " ";
        
        context->warning( SRC_INFO, ss<<SS::c_str() );
        
        return Enum();
    }
    
    std::string Write( Enum const & value, Context const * context ) const
    {
        for( std::pair< Enum, const char * > const & val : StringIO< Enum >::Values )
        {
            if( value == val.first )
                return val.second;
        }
        
        //context->warning( SRC_INFO, Warning( "Can not write value of type '", typeid( Enum ).name(), "' to string." ) );
        std::stringstream ss;
        ss << (typename std::underlying_type< Enum >::type)value;
        return ss.str();
    }
};

}
