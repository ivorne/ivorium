#pragma once

#include "Destination_Connector.hpp"

namespace iv
{

template< class T >
class Destination_AnimNode : public AnimNode< T >, public Destination_Connector< T >
{
public:
using AnimNode< T >::instance;
ClientMarker cm;

    Destination_AnimNode( Instance * inst, T const & initial_value ) :
        AnimNode< T >( inst, initial_value ),
        Destination_Connector< T >( inst, this, nullptr ),
        cm( inst, this, "Destination_AnimNode" )
    {
        this->cm.inherits( this->AnimNode< T >::cm, this->Destination_Connector< T >::cm );
    }
    
    Destination_AnimNode( Instance * inst, Attr< T > * destination, T const & initial_value ) :
        AnimNode< T >( inst, initial_value ),
        Destination_Connector< T >( inst, this, destination ),
        cm( inst, this, "Destination_AnimNode" )
    {
        this->cm.inherits( this->AnimNode< T >::cm, this->Destination_Connector< T >::cm );
    }
};

}
