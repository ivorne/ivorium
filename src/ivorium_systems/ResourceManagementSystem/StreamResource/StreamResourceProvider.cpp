#include "StreamResourceProvider.hpp"
#include "../../Defs.hpp"

namespace iv
{

void StreamResourceProvider::UnregisterAllTypes()
{
    if( !StreamResourceProvider::Markers )
        return;
    
    delete StreamResourceProvider::Markers;     // This should delete all members via unique_ptr.
    StreamResourceProvider::Markers = nullptr;
}

StreamResource::StreamResource( Instance * inst, StreamResourceProvider const * provider, ResourcePath const & path ) :
    cm( inst, this, "StreamResource", ClientMarker::Status() ),
    inst( inst ),
    provider( provider ),
    path( path )
{
}

void StreamResource::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "StreamResource" );
    
    auto row = view->Table( DebugTable ).Row( this );
    row.Column( "path", this->path );
}

Instance * StreamResource::instance() const
{
    return this->inst;
}

void StreamResource::with_resource_stream( std::function< void( std::istream & ) > const & f ) const
{
    this->provider->with_stream( this->path, f );
}

ResourcePath StreamResource::resource_path() const
{
    return this->path;
}

std::unordered_multimap< std::type_index, std::unique_ptr< StreamResourceProvider::Marker > > * StreamResourceProvider::Markers;

StreamResourceProvider::StreamResourceProvider( Instance * inst, size_t priority ) :
    ResourceProvider( inst, priority ),
    cm( inst, this, "StreamResourceProvider" )
{
    this->cm.inherits( this->ResourceProvider::cm );
}

bool StreamResourceProvider::has_path( ResourcePath const & path, std::type_index type )
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

void StreamResourceProvider::each_resource( std::type_index type, std::function< void( ResourcePath const & ) > const & f )
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

void StreamResourceProvider::each_type( std::function< void( std::type_index type ) > const & f )
{
    if( !Markers )
        return;
    
    for( auto it = Markers->begin(); it != Markers->end(); ++it )
        f( it->second->GetType() );
}

std::pair< void *, Instance * > StreamResourceProvider::create_resource( ResourcePath const & path, std::type_index type, ResourcesRoot * parent )
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
