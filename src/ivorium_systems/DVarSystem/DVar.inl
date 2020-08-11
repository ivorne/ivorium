namespace iv
{

template< class Type >
DVar< Type >::DVar( Instance * inst, DVarIdT< Type > id ) :
    cm( inst, this, "DVar" ),
    inst( inst ),
    dvs( inst->getSystemContainer()->getSystem< DVarSystem >() ),
    id( id )
{
}

template< class Type >
DVar< Type >::~DVar()
{
    if( this->dvs && this->_on_changed )
        this->dvs->remove_listener( this, this->id );
}

template< class Type >
Instance * DVar< Type >::instance()
{
    return this->inst;
}

template< class Type >
void DVar< Type >::dvar( DVarIdT< Type > id )
{
    if( this->dvs && this->id.valid() && this->_on_changed )
        this->dvs->remove_listener( this, this->id );
        
    this->id = id;
    
    if( this->dvs && this->id.valid() && this->_on_changed )
        this->dvs->add_listener( this, this->id );
        
    if( this->_on_changed )
    {
        this->cached = this->get_uncached();
        this->_on_changed( this->get() );
    }
}

template< class Type >
DVarIdT< Type > DVar< Type >::dvar() const
{
    return this->id;
}

template< class Type >
Type DVar< Type >::get_uncached()
{
    if( !this->id.valid() )
        return Type();
    
    if( this->dvs )
        return this->dvs->get( this->id );
    else
        return this->id.initial();
}

template< class Type >
Type DVar< Type >::get()
{
    if( !this->id.valid() )
        return Type();
    
    if( this->_on_changed )
        return this->cached;
    else
        return this->get_uncached();
}

template< class Type >
void DVar< Type >::on_changed( std::function< void( Type const & newval ) > const & fun, bool call_immediately )
{
    if( this->dvs && this->id.valid() )
    {
        if( this->_on_changed && !fun )
            this->dvs->remove_listener( this, this->id );
            
        if( fun && !this->_on_changed )
            this->dvs->add_listener( this, this->id );
    }
    
    if( fun )
        this->cached = this->get_uncached();
    else
        this->cached = Type();
        
    this->_on_changed = fun;
    
    if( call_immediately && this->id.valid() && this->_on_changed )
        this->_on_changed( this->get() );
}

template< class Type >
void DVar< Type >::on_dvar_changed( DVarId id, std::type_index type, void * value )
{
    if( this->_on_changed )
    {
        if( type != typeid( Type ) )
        {
            this->cm.warning( SRC_INFO, "Unexpected DVar type." );
            return;
        }
        
        this->cached = *reinterpret_cast< Type * >( value );
        this->_on_changed( this->get() );
    }
}


}
