#include "VirtualResourceProvider.hpp"

namespace iv
{

std::unordered_multimap< std::type_index, std::unique_ptr< VirtualResourceProvider::Marker > > * VirtualResourceProvider::Markers;

void VirtualResourceProvider::UnregisterAllTypes()
{
    if( !VirtualResourceProvider::Markers )
        return;
    
    delete VirtualResourceProvider::Markers;        // This should delete all members via unique_ptr.
    VirtualResourceProvider::Markers = nullptr;
}

VirtualResourceProvider::VirtualResourceProvider( Instance * inst, size_t priority ) :
    ResourceProvider( inst, priority ),
    cm( inst, this, "VirtualResourceProvider" )
{
    this->cm.inherits( this->ResourceProvider::cm );
}

bool VirtualResourceProvider::has_path( ResourcePath const & path, std::type_index type )
{
    if( !Markers )
        return false;
    
    auto const & [ begin, end ] = Markers->equal_range( type );
    for( auto it = begin; it != end; ++it )
    {
        auto & marker = it->second;
        if( marker->HasResource( this, path ) )
            return true;
    }
    
    return false;
}

void VirtualResourceProvider::each_resource( std::type_index type, std::function< void( ResourcePath const & ) > const & f )
{
    if( !Markers )
        return;
    
    auto const & [ begin, end ] = Markers->equal_range( type );
    for( auto it = begin; it != end; ++it )
    {
        auto & marker = it->second;
        marker->EachResource( this, f );
    }
}

void VirtualResourceProvider::each_type( std::function< void( std::type_index type ) > const & f )
{
    if( !Markers )
        return;
    
    for( auto it = Markers->begin(); it != Markers->end(); ++it )
        f( it->second->GetType() );
}

std::pair< void *, Instance * > VirtualResourceProvider::create_resource( ResourcePath const & path, std::type_index type, ResourcesRoot * parent )
{
    if( !Markers )
        return std::pair< void *, Instance * >( nullptr, nullptr );
    
    auto const & [ begin, end ] = Markers->equal_range( type );
    for( auto it = begin; it != end; ++it )
    {
        auto & marker = it->second;
        if( marker->HasResource( this, path ) )
            return marker->MakeResource( this, parent, path );
    }
    
    return std::pair< void *, Instance * >( nullptr, nullptr );
}

}
