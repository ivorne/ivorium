namespace iv
{

template< class TResource >
bool ResourceManagementSystem::has_path( ResourcePath path )
{
    ResourcePath res_path( path );
    return this->choose_provider( res_path, typeid( TResource ) );
}

template< class TResource >
TResource const * ResourceManagementSystem::get( ResourcePath res_path )
{
    //----------- try to find existing instance --------------------
    {
        auto equal_range = this->resources.equal_range( res_path );
        for( auto it = equal_range.first; it != equal_range.second; ++it )
        {
            Resource & res = it->second;
            if( res.type == typeid( TResource ) )
                return reinterpret_cast< TResource * >( res.client );
        }
    }
    
    //---------------- create the instance -------------------------
    {
        // select provider
        ResourceProvider * provider = this->choose_provider( res_path, typeid( TResource ) );
        if( !provider )
        {
            this->warning( SRC_INFO, "Path '", res_path, "' can not be resolved. No registered provider provides this path for type '", typeid( TResource ).name(), "'." );
            return nullptr;
        }
        
        // create client
        std::pair< void *, Instance * > created = provider->create_resource( res_path, typeid( TResource ), &this->root );
        if( !created.first )
        {
            this->warning( SRC_INFO, "Resource client construction failed." );
            return nullptr;
        }
        
        // add to resource set
        Resource res;
        res.type = typeid( TResource );
        res.client = created.first;
        res.inst = created.second;
        this->resources.insert( std::make_pair( res_path, res ) );
        
        // return
        TResource * client = reinterpret_cast< TResource * >( created.first );
        return client;
    }
}

template< class TResource >
void ResourceManagementSystem::each_resource( std::function< void( ResourcePath const & path ) > const & f )
{
    for( auto & p_provider : this->providers )
        p_provider.second->each_resource( typeid( TResource ), f );
}

}
