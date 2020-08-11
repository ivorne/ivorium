namespace iv
{

template< class TResource >
bool ResourceAccess::has_path( ResourcePath const & path )
{
    if( !this->rms )
        return false;
        
    return this->rms->has_path< TResource >( path );
}

template< class TResource >
TResource const * ResourceAccess::get( ResourcePath const & path )
{
    if( !this->rms )
    {
        this->cm.warning( SRC_INFO, "ResourceManagementSystem does not exist." );
        return nullptr;
    }
    
    Resource_LogTrace _trace( path );
    return this->rms->get< TResource >( path );
}

template< class TResource >
void ResourceAccess::each_resource( std::function< void( ResourcePath const & path ) > const & )
{
    if( !this->rms )
    {
        this->cm.warning( SRC_INFO, "ResourceManagementSystem does not exist." );
        return;
    }
    
    return this->rms->each_resource< TResource >();
}

}
