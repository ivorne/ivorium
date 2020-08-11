namespace iv
{

template< class T >
AnimNode< T >::AnimNode( Instance * inst, T const & initial_value ) :
    AnimNodeI( inst ),
    cm( inst, this, "AnimNode", ClientMarker::Status() ),
    _target( initial_value ),
    _last_target( initial_value )
{
    static_assert( !std::is_pointer<T>::value, "I don't think that AnimNode should have a pointer as template argument." );
    this->cm.inherits( this->AnimNodeI::cm );
}

template< class T >
void AnimNode< T >::status( TableDebugView * view )
{
    static TableId DebugTable = TableId::create( "AnimNode" );
    
    auto row = view->Table( AnimNode_DebugTable ).Row( this );
    row.Column( "target", this->_target );
    row.Column( "last_target", this->_last_target );
}

template< class T >
AnimNode< T > * AnimNode< T >::label( std::string const & val )
{
    this->AnimNodeI::label( val );
    return this;
}

template< class T >
std::string const & AnimNode< T >::label() const
{
    return this->AnimNodeI::label();
}

template< class T >
void AnimNode< T >::StringIO_SetTarget( const char * source, Context const * context )
{
    this->Target( iv::StringIO_Read< T >( source, context ) );
}

template< class T >
std::string AnimNode< T >::StringIO_GetTarget( Context const * context )
{
    return iv::StringIO_Write< T >( this->Target(), context );
}

template< class T >
Anim_float AnimNode< T >::Speed() const
{
    return this->AnimNodeI::Speed();
}

template< class T >
AnimNode< T > * AnimNode< T >::Speed( Anim_float val )
{
    this->AnimNodeI::Speed( val );
    return this;
}

template< class T >
AnimNode< T > * AnimNode< T >::Speed_inf()
{
    this->AnimNodeI::Speed_inf();
    return this;
}

template< class T >
AnimNode< T > * AnimNode< T >::Target( T const & target )
{
    if( this->_target == target )
        return this;
    
    this->Activate();
    this->_target = target;
    
    // next time node arrives to destination, connector should be notified again
    this->in_target_notified = false;
    
    // inform children that target changed
    this->anim_eachChild(
        [&]( AnimConnector * child )
        {
            child->parentChanged_Set();
        }
    );
    
    return this;
}

template< class T >
T const & AnimNode< T >::Target()
{
    return this->_target;
}

template< class T >
T const & AnimNode< T >::LastTarget()
{
    return this->_last_target;
}

template< class T >
bool AnimNode< T >::IsInTarget()
{
    return this->Distance() == Anim_float( 0.0 ) && this->_last_target == this->_target;
}

template< class T >
bool AnimNode< T >::IsInTarget( T const & target )
{
    return this->Distance() == Anim_float( 0.0 ) && this->_last_target == this->_target && this->_last_target == target;
}

template< class T >
void AnimNode< T >::UpdateLastTarget()
{
    this->_last_target = this->_target;
}

template< class T >
bool AnimNode< T >::TargetStabilized()
{
    return this->_last_target == this->_target;
}

}
