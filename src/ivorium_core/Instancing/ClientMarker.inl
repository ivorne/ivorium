namespace iv
{

template< class ClientType >
ClientMarker::ClientMarker( Instance * inst, ClientType * client, const char * name ) :
    inst( inst ),
    _name( name ),
    _client_ptr( reinterpret_cast< void * >( client ) ),
    _client_type( typeid( ClientType ) ),
    _marker_id( ClientMarkerIds::UniqueId( name ) ),
    status_printer_w( nullptr ),
    inh_child( nullptr ),
    own_parent( nullptr )
{
    // register to instance
    this->inst->client_register( this );
}

template< class ClientType >
ClientMarker::ClientMarker( Instance * inst, ClientType * client, const char * name, Status ) :
    inst( inst ),
    _name( name ),
    _client_ptr( reinterpret_cast< void * >( client ) ),
    _client_type( typeid( ClientType ) ),
    _marker_id( ClientMarkerIds::UniqueId( name ) ),
    status_printer_w( &ClientMarker::StatusPrinterWImpl< ClientType > ),
    inh_child( nullptr ),
    own_parent( nullptr )
{
    // register to instance
    this->inst->client_register( this );
}

template< class ClientType >
void ClientMarker::StatusPrinterWImpl( void * client_ptr, TableDebugView * view )
{
    ClientType * client_ptr_T = reinterpret_cast< ClientType * >( client_ptr );
    client_ptr_T->status( view );
}

template< class TypedClient >
TypedClient * ClientMarker::client() const
{
    if( this->_client_type == typeid( TypedClient ) )
        return reinterpret_cast< TypedClient * >( this->_client_ptr );
    else
        return nullptr;
}

template< class ... Rest >
void ClientMarker::inherits( ClientMarker & parent, Rest & ... rest )
{
    if( parent.instance() != this->instance() )
    {
        this->warning( SRC_INFO, "ClientMarkers from different instances can not inherit each other." );
        return;
    }
    
    this->inh_parents.insert( &parent );
    parent.inh_child = this;
    this->inherits( rest... );
}

template< class ... Rest >
void ClientMarker::owns( ClientMarker & parent, Rest & ... rest )
{
    if( parent.instance() != this->instance() )
    {
        this->warning( SRC_INFO, "ClientMarkers from different instances can not inherit each other." );
        return;
    }
    
    this->own_children.insert( &parent );
    parent.own_parent = this;
    this->owns( rest... );
}

}
