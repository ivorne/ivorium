namespace iv
{

template< class T >
DVarProperty< T >::DVarProperty( Instance * inst, DVarIdT< T > id ) :
    cm( inst, this, "DVarProperty<>" ),
    inst( inst ),
    dvs( inst->getSystem< DVarSystem >() ),
    _dvar_id( id ),
    _on_changed(),
    _explicit_value()
{
    if( this->dvs )
    {
        this->_cached_value = this->dvs->template get< T >( this->_dvar_id );
        this->dvs->add_listener( this, this->_dvar_id );
    }
    else
    {
        this->_cached_value = this->_dvar_id.initial();
    }
}

template< class T >
DVarProperty< T >::~DVarProperty()
{
    if( this->dvs )
        this->dvs->remove_listener( this, this->_dvar_id );
}

template< class T >
Instance * DVarProperty< T >::instance()
{
    return this->inst;
}

template< class T >
void DVarProperty< T >::value( std::optional< T > const & val )
{
    this->_explicit_value = val;
    if( this->_on_changed )
        this->_on_changed( this->value() );
}

template< class T >
T DVarProperty< T >::value()
{
    if( this->_explicit_value.has_value() )
        return this->_explicit_value.value();
    else
        return this->_cached_value;
}

template< class T >
void DVarProperty< T >::on_dvar_changed( DVarId id, std::type_index type, void * value )
{
    if( type != typeid( T ) )
    {
        this->cm.warning( SRC_INFO, "Wrong type of DVar." );
        return;
    }
    
    this->_cached_value = *reinterpret_cast< T * >( value );
    
    if( this->_on_changed )
        this->_on_changed( this->value() );
}

template< class T >
void DVarProperty< T >::on_changed( std::function< void( T const & newval ) > const & fun, bool call_immediately )
{
    this->_on_changed = fun;
    if( call_immediately && this->_on_changed )
        this->_on_changed( this->value() );
}

}
