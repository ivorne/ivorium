namespace iv
{
    
template< class RT >
SingularResource< RT >::SingularResource( Instance * inst, ResourcePath const & path ) :
    cm( inst, this, "SingularResource" ),
    access( inst ),
    resource( nullptr )
{
    this->cm.owns( this->access.cm );
    this->path( path );
}

template< class RT >
Instance * SingularResource< RT >::instance() const
{
    return this->access.instance();
}

template< class RT >
void SingularResource< RT >::path( ResourcePath const & path )
{
    if( path == ResourcePath() || path == ResourcePath( "/" ) )
        this->resource = nullptr;
    else
        this->resource = this->access.template get< RT >( path );
}

template< class RT >
RT const * SingularResource< RT >::get() const
{
    return this->resource;
}

template< class RT >
RT const & SingularResource< RT >::operator*() const
{
    return *this->resource;
}

template< class RT >
RT const * SingularResource< RT >::operator->() const
{
    return this->resource;
}

}
