namespace iv
{

template< class TypedSystem >
TypedSystem * SystemContainer::getSystem() const
{
    // system must inherit System
    static_assert
        (
            std::is_base_of< System, TypedSystem >::value, 
            "TypedSystem must be a descendant of ivorium::System."
        );
    
    // check if system exists
    if( !this->systems.count( typeid( TypedSystem ) ) )
    {
        if( this->systems.count( typeid( InstanceSystem ) ) )
        {
            auto is = this->systems.at( typeid( InstanceSystem ) );
            is->warning( SRC_INFO, "Requested system ", typeid( TypedSystem ).name(), " does not exist." );
        }
        return nullptr;
    }
    
    // return system
    return dynamic_cast< TypedSystem * >( this->systems.at( typeid(TypedSystem) ) );
}

template< class TypedSystem, typename... Args >
TypedSystem * SystemContainer::createOwnSystem( Args... constructor_arguments )
{
    // system must inherit System
    static_assert
        (
            std::is_base_of<System, TypedSystem>::value, 
            "TypedSystem must be a descendant of ivorium::System."
        );
    
    // not-own system exists -> remove it
    if( this->systems.count( typeid( TypedSystem ) ) && !this->own_systems.count( this->systems[ typeid( TypedSystem ) ] ) )
    {
        this->removeSystem< TypedSystem >();
    }
    
    // own system does not exist -> create it
    if( !this->systems.count( typeid( TypedSystem ) ) )
    {
        // create the system
        TypedSystem * system = new TypedSystem( this, constructor_arguments... );
    
        // increase refcounter
        system->retain();
        
        // save system
        this->systems[ typeid( TypedSystem ) ] = system;
        
        // mark it as own
        this->own_systems.insert( system );
    }
    
    // return system
    return this->getSystem< TypedSystem >();
}
    
template< class TypedSystem >
TypedSystem * SystemContainer::addForeignSystem( TypedSystem * system  )
{
    // system must inherit System
    static_assert
        (
            std::is_base_of<System, TypedSystem>::value, 
            "TypedSystem must be a descendant of ivorium::System."
        );
    
    // check if exists
    if( this->systems.count( typeid( TypedSystem ) ) )
        return this->getSystem< TypedSystem >();
    
    // increase refcounter
    system->retain();
    
    // save system
    this->systems[ typeid( TypedSystem ) ] = system;
    
    // return system
    return system;
}

template< class TypedSystem >
void SystemContainer::removeSystem()
{
    // system must inherit System
    static_assert
        (
            std::is_base_of<System, TypedSystem>::value, 
            "TypedSystem must be a descendant of ivorium::System."
        );
    
    // check if system exists
    if( !this->systems.count( typeid( TypedSystem ) ) )
    {
        runtime_warning( SRC_INFO, "SystemContainer does not contain this type of system." );
        return;
    }
 
    // get system
    System * system = this->systems[ typeid( TypedSystem ) ];
 
    // remove from container
    this->systems.erase( typeid( TypedSystem ) );
    this->own_systems.erase( system );
    
    // decrease refcounter
    system->release();
}

}
