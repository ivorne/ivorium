namespace iv
{

//=================== Link ==============================
template< class Target >
Link< Target >::Link() :
    _target( nullptr )
{
}

template< class Target >
Link< Target >::Link( Target * target ) :
    _target( target )
{
}

template< class Target >
void Link< Target >::set( Target * tgt )
{
    this->_target = tgt;
}

template< class Target >
Target * Link< Target >::get()
{
    return this->_target;
}

template< class Target >
Target * Link< Target >::operator->()
{
    return this->_target;
}

template< class Target >
Target * Link< Target >::operator*()
{
    return this->_target;
}

//=================== VLink ==============================
template< class Target >
VLink::VLink( Target * target ) :
    _type( typeid( std::decay_t< Target >() ) ),
    _target( reinterpret_cast< void * >( target ) )
{
}

template< class Target >
Target * VLink::get()
{
    if( this->_type != std::decay_t< Target >() )
        return nullptr;
    return reinterpret_cast< Target * >( this->_target );
}

template< class Target >
void VLink::set( Target * target )
{
    this->_type = typeid( std::decay_t< Target >() );
    this->_target = reinterpret_cast< void * >( target );
}

}
