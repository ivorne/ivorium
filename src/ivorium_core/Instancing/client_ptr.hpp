#pragma once

#include <unordered_set>
#include <unordered_map>
#include <functional>

#include "instance_ptr.hpp"

namespace iv
{
    
class Instance;

/**
 *  \ingroup ComponentSystem
 */
template<class TypedClient>
class client_ptr : public instance_ptr_interface
{
public:
                                client_ptr      ( TypedClient * client = nullptr );
                                client_ptr      ( client_ptr<TypedClient> const & other );
                                ~client_ptr     ( );

    client_ptr<TypedClient> &   operator =      ( client_ptr<TypedClient> const & other );
    client_ptr<TypedClient> &   operator =      ( TypedClient * client );
    
    
    bool                        operator ==     ( client_ptr<TypedClient> const & other );
    bool                        operator !=     ( client_ptr<TypedClient> const & other );
    bool                        operator <      ( client_ptr<TypedClient> const & other );
    bool                        operator >      ( client_ptr<TypedClient> const & other );
    bool                        operator <=     ( client_ptr<TypedClient> const & other );
    bool                        operator >=     ( client_ptr<TypedClient> const & other );
                        
    TypedClient *               get             ( ) const;
    TypedClient &               operator *      ( ) const;
    TypedClient *               operator ->     ( ) const;
                                operator bool   ( ) const;

protected:
    virtual void                invalidate      ( Instance * inst ) override;
private:
    TypedClient *               client;
};

/**
    \ingroup ComponentSystem
    Careful - there is no guarantee about where the callback might get called.
    So do as little as possible from the callback, preferably just set some dirty flag and process the change from better controlled stack.
*/
template<class TypedClient>
class callback_client_ptr : public client_ptr< TypedClient >
{
public:
using client_ptr< TypedClient >::client_ptr;

    callback_client_ptr<TypedClient> &  operator=( callback_client_ptr<TypedClient> const & other );
    callback_client_ptr<TypedClient> &  operator=( TypedClient * instance );
    
    void                    set_change_callback( std::function< void() > const & );
    
protected:
    virtual void            invalidate( Instance * inst ) override;
    
private:
    std::function< void() > _change_callback;
};

}

#include "InstanceSystem.hpp"
#include "client_ptr.inl"

namespace std
{
template< class TypedClient >
struct hash< iv::client_ptr< TypedClient > >
{
    size_t operator()( iv::client_ptr< TypedClient > & ptr )
    {
        return std::hash< TypedClient * >()( ptr.get() );
    }
};
}
