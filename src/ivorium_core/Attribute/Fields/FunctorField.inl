namespace iv
{

template< class T >
FunctorField< T >::FunctorField( Instance * inst, std::function< void( bool ) > const & OnChanged ) :
    Field< T >( inst ),
    change_reporter( OnChanged )
{
}

template< class T >
void FunctorField< T >::OnChange( std::function< void( bool real ) > const & f )
{
    this->change_reporter = f;
    if( this->change_reporter )
        this->change_reporter( false );
}

template< class T >
T FunctorField< T >::Get()
{
    return this->Field< T >::Get();
}

template< class T >
void FunctorField< T >::OnChanged( bool real )
{
    if( this->change_reporter )
        this->change_reporter( real );
}

template< class T >
void FunctorField< T >::Modify( T const & val )
{
    IV_MEMCHECK( val );
    this->Field< T >::Modify( val );
}

}
