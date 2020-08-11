namespace iv
{

template< class Me >
runtime_id::Dictionary * RuntimeId< Me >::dict()
{
    return Me::Dictionary.dict();
}

template< class Me >
RuntimeId< Me >::RuntimeId() :
    _runtime_value( runtime_id::Dictionary::InvalidRuntimeId )
{
}

template< class Me >
RuntimeId< Me >::RuntimeId( int runtime ) :
    _runtime_value( dict()->runtime_validate( runtime ) )
{
}

template< class Me >
RuntimeId< Me >::RuntimeId( const char * persistent ) :
    _runtime_value( dict()->persistent_to_runtime( persistent ) )
{
}

template< class Me >
RuntimeId< Me >::RuntimeId( const char * persistent, Context * context ) :
    _runtime_value( dict()->persistent_to_runtime( persistent ) )
{
    if( !this->valid() )
        RuntimeId_WarningIdDoesNotExist( context, Me::TypeName, persistent );
}

template< class Me >
Me RuntimeId< Me >::create( const char * persistent_name )
{
    return Me( dict()->create( persistent_name, Me::TypeName ) );
}

template< class Me >
void RuntimeId< Me >::lock()
{
    dict()->lock();
}

template< class Me >
size_t RuntimeId< Me >::ids_count()
{
    return dict()->ids_count();
}

template< class Me >
int RuntimeId< Me >::runtime_value() const
{
    return this->_runtime_value;
}

template< class Me >
std::string RuntimeId< Me >::persistent_value() const
{
    return dict()->runtime_to_persistent( this->_runtime_value );
}

template< class Me >
bool RuntimeId< Me >::valid() const
{
    return this->_runtime_value != runtime_id::Dictionary::InvalidRuntimeId;
}

template< class Me >
bool RuntimeId< Me >::operator==( Me const & other ) const
{
    return this->_runtime_value == other._runtime_value;
}

template< class Me >
bool RuntimeId< Me >::operator!=( Me const & other ) const
{
    return this->_runtime_value != other._runtime_value;
}

template< class Me >
bool RuntimeId< Me >::operator<( Me const & other ) const
{
    return this->_runtime_value < other._runtime_value;
}

//------ io --------------------
template< class Me >
std::ostream & operator<<( std::ostream & out, RuntimeId< Me > const & id )
{
    return ( out << id.persistent_value() );
}

}
