namespace iv
{

template< class TypedSystem >
TypedSystem * Instance::getSystem() const
{
    return this->getSystemContainer()->getSystem< TypedSystem >();
}

template< class TypedClient >
void Instance::Debug_ForeachClient( std::function< void( TypedClient * ) > const & lambda )
{
    for( ClientMarker const * cm : this->_clients )
        if( cm->client_type() == typeid( TypedClient ) )
            lambda( cm->client< TypedClient >() );
}

}
