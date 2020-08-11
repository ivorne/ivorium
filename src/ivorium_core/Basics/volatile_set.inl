namespace iv
{

template< class Key, class Hash, class Pred >
void volatile_set< Key, Hash, Pred >::insert( Key const & val )
{
    if( this->iterators.size() )
        this->toinsert.insert( val );
    else
        this->items.insert( val );
}

template< class Key, class Hash, class Pred >
void volatile_set< Key, Hash, Pred >::erase( Key const & val )
{
    this->toinsert.erase( val );
    
    auto it = this->items.find( val );
    
    if( it == this->items.end() )
        return;
    
    for( auto & iter : this->iterators )
        if( iter == it )
            ++iter;

    this->items.erase( val );
}

template< class Key, class Hash, class Pred >
void volatile_set< Key, Hash, Pred >::foreach( std::function< void( Key const & ) > const & fun )
{
    auto & it = this->lock_begin();
    
    while( it != this->items.end() )
    {
        Key const & item = *it;
        ++it;
        fun( item );
    }
    
    this->unlock();
}

template< class Key, class Hash, class Pred >
typename volatile_set<Key,Hash,Pred>::iterator & volatile_set< Key, Hash, Pred >::lock_begin()
{
    this->iterators.push_back( this->items.begin() );
    return this->iterators.back();
}


template< class Key, class Hash, class Pred >
typename volatile_set<Key,Hash,Pred>::iterator & volatile_set< Key, Hash, Pred >::lock_find( Key const & val )
{
    this->iterators.push_back( this->items.find( val ) );
    return this->iterators.back();
}

template< class Key, class Hash, class Pred >
void volatile_set< Key, Hash, Pred >::unlock()
{
    this->iterators.pop_back();
    
    if( this->iterators.empty() )
    {
        this->items.insert( this->toinsert.begin(), this->toinsert.end() );
        this->toinsert.clear();
    }
}

template< class Key, class Hash, class Pred >
typename volatile_set<Key,Hash,Pred>::iterator volatile_set< Key, Hash, Pred >::end()
{
    return this->items.end();
}

template< class Key, class Hash, class Pred >
int volatile_set< Key, Hash, Pred >::count( Key const & val )
{
    return this->items.count( val ) || this->toinsert.count( val );
}

template< class Key, class Hash, class Pred >
size_t volatile_set< Key, Hash, Pred >::size()
{
    return this->items.size();
}


template< class Key, class Hash, class Pred >
bool volatile_set< Key, Hash, Pred >::empty()
{
    return this->items.empty();
}

}
