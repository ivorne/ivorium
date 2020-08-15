namespace iv
{

template< class Component >
World< Component >::World( iv::Instance * inst ) :
    cm( inst, this, "World" ),
    inst( inst ),
    _components()
{
}

template< class Component >
iv::Instance * World< Component >::instance() const
{
    return this->inst;
}

template< class Component >
void World< Component >::Register( Component * component )
{
    this->_components.insert( component );
    this->Component_Registered( component );
}

template< class Component >
void World< Component >::Unregister( Component * component )
{
    this->_components.erase( component );
    this->Component_Unregistered( component );
}

template< class Component >
std::unordered_set< Component * > const & World< Component >::components()
{
    return this->_components;
}

}
