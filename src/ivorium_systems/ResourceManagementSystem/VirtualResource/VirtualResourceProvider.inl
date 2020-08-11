namespace iv
{

template< class TVirtualSubprovider, class TClient, class TParserClient >
void VirtualResourceProvider::RegisterType()
{
    static_assert( std::is_base_of< TClient, TParserClient >::value, "TParserClient must inherit TClient." );
    
    // init directory
    if( !Markers )
        Markers = new std::unordered_multimap< std::type_index, std::unique_ptr< Marker > >;
    
    Markers->insert( std::pair< std::type_index, std::unique_ptr< Marker > >( typeid( TClient ), new TMarker< TVirtualSubprovider, TClient, TParserClient > ) );
}

template< class TVirtualSubprovider, class TClient, class TParserClient >
std::pair< void *, Instance * > VirtualResourceProvider::TMarker< TVirtualSubprovider, TClient, TParserClient >::MakeResource( VirtualResourceProvider * provider, ResourcesRoot * root, ResourcePath const & path ) const
{
    auto subprovider = this->GetSubprovider( provider );
    //auto client = inst->createAndBindInstance< TParserClient >( (VirtualResourceProvider const *)provider, (TVirtualSubprovider const *)subprovider, path );
    auto client = root->heap.create< I< TParserClient > >( "", root->instance(), (VirtualResourceProvider const *)provider, (TVirtualSubprovider const *)subprovider, path );
    return std::pair( reinterpret_cast< void * >( static_cast< TClient * >( client ) ), client->instance() );
}

template< class TVirtualSubprovider, class TClient, class TParserClient >
void VirtualResourceProvider::TMarker< TVirtualSubprovider, TClient, TParserClient >::EachResource( VirtualResourceProvider * provider, std::function< void( ResourcePath const & path ) > const & f ) const
{
    auto subprovider = this->GetSubprovider( provider );
    subprovider->each_resource( f );
}

template< class TVirtualSubprovider, class TClient, class TParserClient >
bool VirtualResourceProvider::TMarker< TVirtualSubprovider, TClient, TParserClient >::HasResource( VirtualResourceProvider * provider, ResourcePath const & path ) const
{
    auto subprovider = this->GetSubprovider( provider );
    return subprovider->has_resource( path );
}

template< class TVirtualSubprovider, class TClient, class TParserClient >
TVirtualSubprovider * VirtualResourceProvider::TMarker< TVirtualSubprovider, TClient, TParserClient >::GetSubprovider( VirtualResourceProvider * provider ) const
{
    auto it = provider->subproviders.find( this );
    if( it == provider->subproviders.end() )
    {
        it = provider->subproviders.insert(
            std::pair(
                this,
                //provider->instance()->createAndBindInstance< TVirtualSubprovider >( static_cast< VirtualResourceProvider const * >( provider ) )
                static_cast< TVirtualSubprovider * >( provider->heap.create< I< TVirtualSubprovider > >( "", provider->instance(), static_cast< VirtualResourceProvider const * >( provider ) ) )
            )
        ).first;
    }
    
    return reinterpret_cast< TVirtualSubprovider * >( it->second );
}

template< class TVirtualSubprovider, class TClient, class TParserClient >
std::type_index VirtualResourceProvider::TMarker< TVirtualSubprovider, TClient, TParserClient >::GetType() const
{
    return typeid( TClient );
}

}
