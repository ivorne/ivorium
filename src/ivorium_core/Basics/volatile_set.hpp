#pragma once

#include <unordered_set>
#include <deque>
#include <functional>

namespace iv
{

/**
    Works like unordered_set, but elements can be inserted and removed while iterating using foreach() method.
    Items that are inserted during iteration will be added after the iteration ends.
    Item erasure is instant.
*/
template< class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key> >
class volatile_set
{
public:
    using iterator = typename std::unordered_set< Key, Hash, Pred >::iterator;
    
    void            insert( Key const & val );
    void            erase( Key const & val );
    
    int             count( Key const & val );
    
    void            foreach( std::function< void( Key const & ) > const & fun );
    
    /// this is stack based; lock_end unlocks (and invalidates) iterator returned by last lock_begin call
    iterator &      lock_begin();
    iterator &      lock_find( Key const & val );
    void            unlock();
    iterator        end();
    
    size_t          size();
    bool            empty();
    
private:
    std::unordered_set< Key, Hash, Pred >   items;
    std::unordered_set< Key, Hash, Pred >   toinsert;
    std::deque< typename decltype( items )::iterator > iterators;
};

}

#include "volatile_set.inl"
