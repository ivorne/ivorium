namespace iv
{

template< class Type >
void DVarManipulator::set( DVarIdT< Type > id, Type const & value )
{
    if( this->dvs )
        this->dvs->set< Type >( this->instance(), id, value );
}

}
