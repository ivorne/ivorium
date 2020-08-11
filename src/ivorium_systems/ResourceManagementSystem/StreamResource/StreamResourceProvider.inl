namespace iv
{

template< class TStreamSubprovider, class TClient, class TParserClient >
void StreamResourceProvider::RegisterType()
{
    static_assert( std::is_base_of< TClient, TParserClient >::value, "TParserClient must inherit TClient." );
    
    // init directory
    if( !StreamResourceProvider::Markers )
        StreamResourceProvider::Markers = new std::unordered_multimap< std::type_index, std::unique_ptr< Marker > >;
    
    StreamResourceProvider::Markers->insert( std::pair< std::type_index, std::unique_ptr< Marker > >( typeid( TClient ), new TMarker< TStreamSubprovider, TClient, TParserClient > ) );
}

template< class TStreamSubprovider, class TClient, class TParserClient >
std::pair< void *, Instance * > StreamResourceProvider::TMarker< TStreamSubprovider, TClient, TParserClient>::MakeResource( StreamResourceProvider * provider, ResourcesRoot * root, ResourcePath const & path ) const
{
    auto subprovider = this->GetSubprovider( provider );
    auto client = root->heap.create< I< TParserClient > >( "", root->instance(), provider, subprovider, path );
    return std::pair( reinterpret_cast< void * >( static_cast< TClient * >( client ) ), client->instance() );
}

template< class TStreamSubprovider, class TClient, class TParserClient >
void StreamResourceProvider::TMarker< TStreamSubprovider, TClient, TParserClient>::EachResource( StreamResourceProvider * provider, std::function< void( ResourcePath const & path ) > const & f ) const
{
    auto subprovider = this->GetSubprovider( provider );
    subprovider->each_resource( f );
}

template< class TStreamSubprovider, class TClient, class TParserClient >
bool StreamResourceProvider::TMarker< TStreamSubprovider, TClient, TParserClient>::HasResource( StreamResourceProvider * provider, ResourcePath const & path ) const
{
    auto subprovider = this->GetSubprovider( provider );
    return subprovider->has_resource( path );
}

template< class TStreamSubprovider, class TClient, class TParserClient >
TStreamSubprovider * StreamResourceProvider::TMarker< TStreamSubprovider, TClient, TParserClient>::GetSubprovider( StreamResourceProvider * provider ) const
{
    auto it = provider->subproviders.find( this );
    if( it == provider->subproviders.end() )
    {
        it = provider->subproviders.insert(
            std::pair(
                this,
                static_cast< TStreamSubprovider * >( provider->heap.create< I< TStreamSubprovider > >( "", provider->instance(), static_cast< StreamResourceProvider const * >( provider ) ) )
            )
        ).first;
    }
    
    return reinterpret_cast< TStreamSubprovider * >( it->second );
}

template< class TStreamSubprovider, class TClient, class TParserClient >
std::type_index StreamResourceProvider::TMarker< TStreamSubprovider, TClient, TParserClient>::GetType() const
{
    return typeid( TClient );
}

}
