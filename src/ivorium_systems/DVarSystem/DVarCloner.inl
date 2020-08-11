namespace iv
{

template< class T >
DVarCloner< T >::DVarCloner( Instance * inst, DVarIdT< T > source, DVarIdT< T > target ) :
    cm( inst, this, "DVarCloner" ),
    inst( inst ),
    dvs( inst->getSystem< DVarSystem >() ),
    source( source ),
    target( target )
{
    if( this->dvs )
    {
        this->dvs->lock( this->instance(), this->target );
        this->dvs->add_listener( this, this->source );
        
        this->dvs->template set< T >( this->instance(), this->target, this->dvs->template get< T >( source ) );
    }
}

template< class T >
DVarCloner< T >::~DVarCloner()
{
    if( this->dvs )
    {
        this->dvs->unlock( this->instance(), this->target );
        this->dvs->remove_listener( this, this->source );
    }
}

template< class T >
Instance * DVarCloner< T >::instance()
{
    return this->inst;
}

template< class T >
void DVarCloner< T >::on_dvar_changed( DVarId id, std::type_index type, void * value ) 
{
    if( type != typeid( T ) )
    {
        this->cm.warning( SRC_INFO, "DVar ", id, " has unexpected type." );
        return;
    }
        
    this->dvs->template set< T >( this->instance(), this->target, *reinterpret_cast< T * >( value ) );
}

}
