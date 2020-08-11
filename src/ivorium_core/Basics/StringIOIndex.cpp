#include "StringIOIndex.hpp"

#include "Context.hpp"

namespace iv
{

std::unordered_map< std::type_index, StringIOIndex::Handler > * StringIOIndex::_handlers;

std::unordered_map< std::type_index, StringIOIndex::Handler > & StringIOIndex::handlers()
{
    if( !_handlers )
        _handlers = new std::unordered_map< std::type_index, StringIOIndex::Handler >;
    return *_handlers;
}

std::any StringIOIndex::Read( std::type_index type, const char * source, Context const * context )
{
    auto & h = handlers();
    auto it = h.find( type );
    if( it == h.end() )
    {
        context->log( SRC_INFO, Defs::Log::Warning, "StringIOIndex does not have StringIO handler registered for type '", type.name(), "'. Register it using StringIOIndex< T >::Register()." );
        return std::any();
    }
    
    return it->second.read( type, source, context );
}

std::string StringIOIndex::Write( std::any const & value, Context const * context )
{
    std::type_index type = value.type();
    auto & h = handlers();
    auto it = h.find( type );
    if( it == h.end() )
    {
        context->log( SRC_INFO, Defs::Log::Warning, "StringIOIndex does not have r/w handler registered for type '", type.name(), "'." );
        return std::string( "<" )+value.type().name()+( "/undecoded>" );
    }
    
    return it->second.write( value, context );
}

}
