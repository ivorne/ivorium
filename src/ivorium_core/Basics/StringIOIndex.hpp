#pragma once

#include "StringIO.hpp"

#include <any>
#include <typeindex>
#include <unordered_map>
#include <string>

namespace iv
{

struct StringIOIndex
{
public:
    static std::any         Read( std::type_index, const char * source, Context const * context );
    static std::string      Write( std::any const & value, Context const * context );

    template< class T >
    static void             Register();
    
private:
    static void             Register_Impl( std::any * ){}   ///< do not register std::any, that would be too confusing
    
    template< class T >
    static void             Register_Impl( T * );

    template< class Type >
    static std::any         DoRead( std::type_index, const char * source, Context const * context );
    
    template< class Type >
    static std::string      DoWrite( std::any const & value, Context const * context );

    struct Handler
    {
        std::any    ( *read )( std::type_index, const char * source, Context const * context );
        std::string ( *write )( std::any const & value, Context const * context );
    };
    
    static std::unordered_map< std::type_index, Handler > & handlers();
    
private:
    static std::unordered_map< std::type_index, Handler > * _handlers;
};


// register it as StriongIO for std::any
template<>
struct StringIO< std::any >
{
    std::any Read( const char * source, Context const * context ) const
    {
        // unimplemented
        return std::any();
    }
    
    std::string Write( std::any const & value, Context const * context ) const
    {
        return StringIOIndex::Write( value, context );
    }
};

}

#include "StringIOIndex.inl"
