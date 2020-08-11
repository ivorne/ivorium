#include "ResourceManagementSystem.hpp"
#include "ResourceProvider.hpp"
#include "VirtualResource/VirtualResourceProvider.hpp"

namespace iv
{

ResourceManagementSystem::ResourceManagementSystem( SystemContainer * sc ):
    System( sc ),
    root( "resources", sc )
{
}

ResourceManagementSystem::~ResourceManagementSystem()
{
}

void ResourceManagementSystem::register_provider( ResourceProvider * provider, size_t priority )
{
    this->providers.insert( std::make_pair( priority, provider ) );
}

void ResourceManagementSystem::unregister_provider( ResourceProvider * provider )
{
    for( auto it = this->providers.begin(); it != this->providers.end(); ++it )
        if( it->second == provider )
        {
            this->providers.erase( it );
            break;
        }
}

void ResourceManagementSystem::status( TextDebugView * context )
{
    auto & out = context->out();
    
    for( auto const & p_provider : this->providers )
    {
        size_t priority = p_provider.first;
        ResourceProvider * provider = p_provider.second;
        
        out << "priority " << priority << " | " << provider->cm.root_name_id() << ":" << std::endl;
        
        std::vector< std::type_index > types;
        
        provider->each_type(
            [ & ]( std::type_index type )
            {
                types.push_back( type );
            }
        );
        
        for( std::type_index type : types )
        {
            out << "    type " << type.name() << ":" << std::endl;
            provider->each_resource(
                type,
                [&]( ResourcePath const & path )
                {
                    out << "        " << path << std::endl;
                }
            );
        }
        
        out << std::endl;
    }
}

ResourceProvider * ResourceManagementSystem::choose_provider( ResourcePath const & path, std::type_index type )
{
    for( auto & p_provider : this->providers )
        if( p_provider.second->has_path( path, type ) )
            return p_provider.second;
    
    return nullptr;
}

}
