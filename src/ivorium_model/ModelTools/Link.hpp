#pragma once

#include <type_traits>
#include <typeindex>

namespace iv
{

/**
    \ingroup ModelTools
    Does not get automatically cleared when target is destroyed.
    Cleanup is responsibility of some kind of data manager which creates and destroys those instances.
    Autocleaned link could be implemented if needed, but should be used only when data manager is not suitable to ensure consistency.
*/
template< class Target >
class Link
{
public:
                        Link();
                        Link( Target * );
    
    // setter
    void                set( Target * );
    
    // getter
    Target *            get();
    Target *            operator->();
    Target *            operator*();
    
private:
    Target * _target;
};

/**
    \ingroup ModelTools
    Link that can link to different types.
    This does not support any kind of concept control and it is not really needed in runtime.
    This is meant to be used for model extendability so complete list of possible target types can not be known so we need to accept that when working with VLinks.
*/
class VLink
{
public:
                        VLink();
                        
                        template< class Target >
                        VLink( Target * );
    
    // setter
    template< class Target >
    void                set( Target * );
    
    // getter
    std::type_index     type();
    
    template< class Target >
    Target *            get();
    
private:
    std::type_index _type;
    void * _target;
};

}

#include "Link.inl"
