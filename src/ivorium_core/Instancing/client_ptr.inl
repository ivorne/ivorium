namespace iv
{

template<class TypedClient>
client_ptr<TypedClient>::client_ptr( TypedClient * client ) :
    client( client )
{
    if( this->client )
        this->addSelfToSC( this->client->instance() );
}

template<class TypedClient>
void client_ptr<TypedClient>::invalidate( Instance * inst )
{
    this->client = nullptr;
}


template<class TypedClient>
client_ptr<TypedClient>::client_ptr( client_ptr<TypedClient> const & other ) :
    client( other.client )
{
    if( this->client )
        this->addSelfToSC( this->client->instance() );
}

template<class TypedClient>
client_ptr<TypedClient>::~client_ptr( )
{
    if( this->client )
        this->removeSelfFromSC( this->client->instance() );
}

template<class TypedClient>
client_ptr<TypedClient> & client_ptr<TypedClient>::operator = ( TypedClient * newClient )
{
    if( this->client )
        this->removeSelfFromSC( this->client->instance() );
    this->client = newClient;
    if( this->client )
        this->addSelfToSC( this->client->instance() );
    return *this;
}

template<class TypedClient>
client_ptr<TypedClient> & client_ptr<TypedClient>::operator = ( client_ptr<TypedClient> const & other )
{
    if( this == &other )
        return *this;
    if( this->client )
        this->removeSelfFromSC( this->client->instance() );
    this->client = other.client;
    if( this->client )
        this->addSelfToSC( this->client->instance() );
    return *this;
}

template<class TypedClient>
bool client_ptr<TypedClient>::operator == ( client_ptr<TypedClient> const & other )
{
    return this->client == other.client;
}

template<class TypedClient>
bool client_ptr<TypedClient>::operator != ( client_ptr<TypedClient> const & other )
{
    return this->client != other.client;
}

template<class TypedClient>
bool client_ptr<TypedClient>::operator < ( client_ptr<TypedClient> const & other )
{
    return this->client < other.client;
}

template<class TypedClient>
bool client_ptr<TypedClient>::operator > ( client_ptr<TypedClient> const & other )
{
    return this->client > other.client;
}

template<class TypedClient>
bool client_ptr<TypedClient>::operator <= ( client_ptr<TypedClient> const & other )
{
    return this->client <= other.client;
}

template<class TypedClient>
bool client_ptr<TypedClient>::operator >= ( client_ptr<TypedClient> const & other )
{
    return this->client >= other.client;
}

template<class TypedClient>
TypedClient * client_ptr<TypedClient>::get ( ) const
{
    return this->client;
}

template<class TypedClient>
TypedClient & client_ptr<TypedClient>::operator * ( ) const
{
    return *this->client;
}

template<class TypedClient>
TypedClient * client_ptr<TypedClient>::operator -> ( ) const
{
    return this->client;
}

template<class TypedClient>
client_ptr<TypedClient>::operator bool ( ) const
{
    return this->client;
}



template<class TypedClient>
callback_client_ptr< TypedClient > & callback_client_ptr< TypedClient >::operator=( callback_client_ptr< TypedClient > const & other )
{
    ( client_ptr< TypedClient > & )( *this ) = ( client_ptr< TypedClient > const & )other;
    if( this->_change_callback )
        this->_change_callback();
    return *this;
}

template<class TypedClient>
callback_client_ptr< TypedClient > & callback_client_ptr< TypedClient >::operator=( TypedClient * client )
{
    ( client_ptr< TypedClient > & )( *this ) = client;
    if( this->_change_callback )
        this->_change_callback();
    return *this;
}

template<class TypedClient>
void callback_client_ptr< TypedClient >::set_change_callback( std::function< void() > const & cb )
{
    this->_change_callback = cb;
}

template<class TypedClient>
void callback_client_ptr< TypedClient >::invalidate( Instance * inst )
{
    client_ptr< TypedClient >::invalidate( inst );
    if( this->_change_callback )
        this->_change_callback();
}

}
