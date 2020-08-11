#pragma once

#include "Instance.hpp"

namespace iv
{

/**
    Object that contains both an Instance and a given client.
    Can be constructed as linked either to a SystemContainer or another Instance.
    Constructed client is considered root client in constructed Instance.
*/
template< class ClientType >
class I final : public Instance, public ClientType
{
public:
using ClientType::instance;

    template< class ... CArgs >
    I( std::string const & name, SystemContainer * sc, CArgs const & ... cargs ) :
        Instance( sc ),
        ClientType( static_cast< Instance * >( this ), cargs ... )
    {
        static_cast< Instance * >( this )->instance_finalize( name, &static_cast< ClientType * >( this )->cm );
    }
    
    template< class ... CArgs >
    I( std::string const & name, Instance * parent, CArgs const & ... cargs ) :
        Instance( parent->getSystemContainer() ),
        ClientType( static_cast< Instance * >( this ), cargs ... )
    {
        static_cast< Instance * >( this )->instance_finalize( name, &static_cast< ClientType * >( this )->cm );
        static_cast< Instance * >( this )->instance_parent( parent ); 
    }
};

}
