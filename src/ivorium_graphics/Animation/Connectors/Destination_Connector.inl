namespace iv
{

template< class T, class Interp >
Destination_ConnectorGI< T, Interp >::Destination_ConnectorGI( Instance * inst, AnimNode< T > * parent, Attr< T > * destination ) :
    AnimConnector( inst ),
    cm( inst, this, "Destination_ConnectorGI" ),
    _accumulated_distance( 0.0 ),
    _parent( nullptr ),
    _destination( destination )
{
    this->cm.inherits( this->AnimConnector::cm );
    this->SetParent( parent );
}

template< class T, class Interp >
Destination_ConnectorGI< T, Interp >::~Destination_ConnectorGI()
{
    if( this->_parent )
        this->_parent->anim_removeChild( this );
}

template< class T, class Interp >
void Destination_ConnectorGI< T, Interp >::SetParent( AnimNode< T > * node )
{
    if( node )
    {
        if( this->_parent )
            this->SetParent( nullptr );
        
        this->_parent = node;
        this->_parent->anim_addChild( this );
        this->Activate();
    }
    else
    {
        this->_parent->anim_removeChild( this );
        this->_parent = nullptr;
        this->Activate();
    }
}

template< class T, class Interp >
void Destination_ConnectorGI< T, Interp >::anim_eachParent( std::function< void( AnimNodeI * ) > const & f )
{
    if( this->_parent )
        f( this->_parent );
}

template< class T, class Interp >
void Destination_ConnectorGI< T, Interp >::anim_parentDisconnect( AnimNodeI * node )
{
    if( this->_parent == node )
    {
        this->_parent->anim_removeChild( this );
        this->_parent = nullptr;
        this->Activate();
    }
}

template< class T, class Interp >
void Destination_ConnectorGI< T, Interp >::SetDestination( Attr< T > * destination )
{
    this->_destination = destination;
    this->Activate();
}

template< class T, class Interp >
void Destination_ConnectorGI< T, Interp >::UpdatePass_Down()
{
    if( !this->_parent || !this->_destination )
    {
        this->_distance = 0.0;
        return;
    }
    
    // advance
    Anim_float distance = this->_parent->Step() + this->_accumulated_distance;
    T tgt = this->_parent->Target();
    T current_value = this->_destination->Get();
    
    auto val_rem = Interp().Interpolate( current_value, tgt, distance );
    this->_accumulated_distance = val_rem.second;
    
    // distance
    auto dist = Interp().Distance( val_rem.first, tgt );
    auto result = dist  - this->_accumulated_distance;
    
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Down | step ", distance, ", value ", current_value, " -> ", val_rem.first, " -> ", tgt, ", accumulated_distance ", this->_accumulated_distance, "." );
    
    // apply
    this->_destination->Modify( &this->cm, val_rem.first );
    this->_distance = result;
}

template< class T, class Interp >
void Destination_ConnectorGI< T, Interp >::UpdatePass_Up()
{
    if( !this->_parent || !this->_destination )
        return;
    
    this->cm.log( SRC_INFO, ::iv::Defs::Log::Animation_ConnectorUpdate, "UpdatePass_Up | distance ", this->_distance, "." );
    
    this->_parent->Connector_UpdateDistance( this->_distance );
}

}
