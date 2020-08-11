#include "RuntimeId.hpp"
#include "static_warning.hpp"
#include <iostream>
#include "Context.hpp"
#include "SS.hpp"

namespace iv
{
   
void RuntimeId_WarningIdDoesNotExist( Context * context, char const * type_name, char const * persistent_name )
{
    context->log( SRC_INFO, iv::Defs::Log::Warning, "RuntimeId ", type_name, " with persistent name '", persistent_name, "' does not exist. Using InvalidId instead." );
}
   
runtime_id::Dictionary * RuntimeIdDictionary::dict()
{
    if( !this->_dict )
        this->_dict = new runtime_id::Dictionary;
    return this->_dict;
}

namespace runtime_id
{

Dictionary::Dictionary() :
    next_free_runtime_id( Dictionary::InvalidRuntimeId + 1 ),
    locked( false )
{
}

void Dictionary::lock()
{
    this->locked = true;
}

int Dictionary::create( const char * persistent_name, char const * type_name )
{
    if( this->locked )
        runtime_warning( SRC_INFO, SS()<<"RuntimeId "<<type_name<<" id already locked (someone does not want new ids to be created). Creating id '"<<persistent_name<<"' anyways, but the functionality associated with whoever locked it may be limited for this id."<<SS::c_str() );
    
    if( this->p2r.count( persistent_name ) )
    {
        //runtime_warning( SRC_INFO, SS()<<"RuntimeId "<<type_name<<": Persistent id '"<<persistent_name<<"' already exists. Definitions will have the same runtime name."<<SS::c_str() );
        
        int runtime_name = this->p2r.at( persistent_name );
        return runtime_name;
    }
    
    int new_id = this->next_free_runtime_id++;
    this->p2r[ persistent_name ] = new_id;
    this->r2p[ new_id ] = persistent_name;
    return new_id;
}

std::string Dictionary::runtime_to_persistent( int runtime )
{
    auto it = this->r2p.find( runtime );
    if( it != this->r2p.end() )
        return it->second;
    else
        return "InvalidRuntimeId";
}

int Dictionary::persistent_to_runtime( const char * persistent )
{
    auto it = this->p2r.find( persistent );
    if( it != this->p2r.end() )
        return it->second;
    else
        return Dictionary::InvalidRuntimeId;
}

int Dictionary::runtime_validate( int runtime )
{
    if( this->r2p.count( runtime ) )
        return runtime;
    else
        return Dictionary::InvalidRuntimeId;
}

size_t Dictionary::ids_count()
{
    return this->next_free_runtime_id;
}

}
}
