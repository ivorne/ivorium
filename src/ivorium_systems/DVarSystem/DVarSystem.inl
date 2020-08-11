namespace iv
{

template< class Type >
void DVarSystem::set( Instance * setter, DVarIdT< Type > id, Type const & value )
{
    Var & var = this->vars[ id ];
    
    if( var.locked && var.locked != setter )
    {
        this->warning( SRC_INFO, "DVar ", id, " is locked to an instance. Can not change it using different setter instance." );
        return;
    }
    
    if( !var.val )
    {
        auto val = new Type;
        var.val = val;
        var.type = typeid( Type );
        var.deleter = [ val ]()
        {
            delete val;
        };
    }
    
    if( var.type != typeid( Type ) )
    {
        this->warning( SRC_INFO, "DVar ", id, " was already set with different type. Skipping this set." );
        return;
    }
    
    ( *reinterpret_cast< Type * >( var.val ) ) = value;
    
    var.listeners.foreach( 
        [&]( DVarListenerI * listener )
        {
            listener->on_dvar_changed( id, var.type, var.val );
        }
    );
}

template< class Type >
Type DVarSystem::get( DVarIdT< Type > id )
{
    Var & var = this->vars[ id ];
    if( !var.val )
    {
        return id.initial();
    }
    
    std::type_index type = typeid( Type );
    if( type != var.type )
    {
        this->warning( SRC_INFO, "Trying to get DVar value using id of different type than with which it was set. There are probably two ids with the same persistent id '", id, "'." );
        return id.initial();
    }
    
    return *reinterpret_cast< Type * >( var.val );
}



}
