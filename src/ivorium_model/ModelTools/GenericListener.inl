namespace iv
{

//========================= GenericListener ============================
template< class Listener >
GenericListener< Listener >::GenericListener( iv::Instance * inst, GenericListener_Index< Listener > * index ) :
    cm( inst, this, "GenericListener" ),
    inst( inst ),
    index( index ),
    initialized( false )
{
    static_assert( std::is_base_of_v< GenericListener< Listener >, Listener >, "Listener must inherit GenericListener< Listener > to allow static down-cast." );
    this->index->AddListener( static_cast< Listener * >( this ) );
}

template< class Listener >
GenericListener< Listener >::~GenericListener()
{
    this->index->RemoveListener( static_cast< Listener * >( this ) );
}

template< class Listener >
iv::Instance * GenericListener< Listener >::instance() const
{
    return this->inst;
}

template< class Listener >
void GenericListener< Listener >::Initialize()
{
    if( this->initialized )
        return;
    
    this->initialized = true;
    this->index->InitializeListener( static_cast< Listener * >( this ) );
}

template< class Listener >
void GenericListener< Listener >::VerifyInitialization()
{
    if( !this->initialized )
        this->cm.warning( SRC_INFO, "Sending event to GenericListener that was not initialized. Call GenericListener< Listener >::Initialize on it after it is fully constructed for proper functionality." );
}

//========================= GenericListener_Index ============================
template< class Listener >
GenericListener_Index< Listener >::GenericListener_Index( iv::Instance * inst ) :
    cm( inst, this, "GenericListener_Index" ),
    inst( inst )
{
}

template< class Listener >
iv::Instance * GenericListener_Index< Listener >::instance() const
{
    return this->inst;
}

template< class Listener >
void GenericListener_Index< Listener >::AddListener( Listener * listener )
{
    this->listeners.insert( listener );
}

template< class Listener >
void GenericListener_Index< Listener >::RemoveListener( Listener * listener )
{
    this->listeners.erase( listener );
}

template< class Listener >
void GenericListener_Index< Listener >::InitializeListener( Listener * listener )
{
    this->Listener_Initialize( listener );
}

template< class Listener >
template< class ... Params >
void GenericListener_Index< Listener >::InvokeListeners( void( Listener::* member )( Params  ... ), Params ... params )
{
    static_assert( std::is_base_of_v< GenericListener< Listener >, Listener >, "Listener should inherit GenericListener< Listener > to allow us to check that it was properly initialized (could be reimplemented, ommiting this QOL feature, if needed)." );
    
    this->listeners.foreach(
        [&]( Listener * listener )
        {
            listener->VerifyInitialization();
            ( listener->*member )( params ... );
        }
    );
}

}
