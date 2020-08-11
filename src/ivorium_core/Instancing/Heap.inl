namespace iv
{

template< class T >
void Heap::DeleterImpl( void * ptr )
{
    delete reinterpret_cast< T * >( ptr );
}

template< class T, class ... CArgs >
T * Heap::create( CArgs const & ... cargs )
{
    T * t = new T( cargs ... );
    this->items.push_back( Item( dynamic_cast< void * >( t ), &DeleterImpl< T > ) );
    return t;
}

template< class T, class ... CArgs >
T * Heap::createInstance( std::string const & name, CArgs const & ... cargs )
{
    return this->create< I< T > >( name, this->inst, cargs ... );
}

template< class T, class ... CArgs >
T * Heap::createClient( CArgs const & ... cargs )
{
    auto client = this->create< T >( this->inst, cargs ... );
    this->cm->owns( client->cm );
    return client;
}

template< class T >
void Heap::destroy( T * ptr )
{
    void * identity = dynamic_cast< void * >( ptr );
    
    auto it = std::find_if( this->items.begin(), this->items.end(), [ identity ]( Item const & item ){ return item.identity == identity; } );
    
    if( it == this->items.end() )
    {
        auto error = "Given object can not be destroyed by this Heap because it was not created by it.";
        if( this->cm )
            this->cm->warning( SRC_INFO, error );
        else
            runtime_warning( SRC_INFO, error );
        
        return;
    }
    
    Item item = *it;
    this->items.erase( it );
    item.deleter( item.identity );
}

}
