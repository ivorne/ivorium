namespace iv
{

template< class Type >
std::any StringIOIndex::DoRead( std::type_index, const char * source, Context const * context )
{
    Type t = StringIO_Read< Type >( source, context );
    return std::any( t );
}

template< class Type >
std::string StringIOIndex::DoWrite( std::any const & value, Context const * context )
{
    return StringIO_Write< Type >( std::any_cast< Type >( value ), context );
}

template< class T >
void StringIOIndex::Register()
{
    Register_Impl( (typename std::decay< T >::type*)nullptr );
}

template< class T >
void StringIOIndex::Register_Impl( T * )
{
    using TDecayed = typename std::decay< T >::type;
    
    auto & handler = handlers()[ typeid( TDecayed ) ];
    
    handler.read = &DoRead< TDecayed >;
    handler.write = &DoWrite< TDecayed >;
}

}
