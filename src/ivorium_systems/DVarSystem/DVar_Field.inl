namespace iv
{

template< class Type >
DVar_Field< Type >::DVar_Field( Instance * inst, DVarIdT< Type > id ) :
    Field< Type >( inst ),
    cm( inst, this, "DVar_Field" ),
    inst( inst ),
    id( id )
{
    this->cm.inherits( this->Field< Type >::cm );
    
    auto dvs = inst->getSystemContainer()->getSystem< DVarSystem >();
    if( dvs )
    {
        auto value = dvs->get< Type >( this->id );
        this->Modify( value );
        dvs->add_listener( this, this->id );
    }
}

template< class Type >
DVar_Field< Type >::~DVar_Field()
{
    auto dvs = inst->getSystemContainer()->getSystem< DVarSystem >();
    if( dvs )
        dvs->remove_listener( this, this->id );
}

template< class Type >
Instance * DVar_Field< Type >::instance() const
{
    return this->inst;
}

template< class Type >
void DVar_Field< Type >::on_dvar_changed( DVarId id, std::type_index type, void * value )
{
    Type & t_value = *reinterpret_cast< Type * >( value );
    this->Modify( t_value );
}

template< class Type >
void DVar_Field< Type >::OnChanged( bool real )
{
    if( !real )
    {
        auto dvs = inst->getSystemContainer()->getSystem< DVarSystem >();
        if( dvs )
        {
            auto value = dvs->get< Type >( this->id );
            this->Modify( value );
        }
    }
}

}
