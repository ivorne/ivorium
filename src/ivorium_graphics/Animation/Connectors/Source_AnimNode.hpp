#pragma once

#include "Source_Connector.hpp"

namespace iv
{

template< class T >
class Source_AnimNode : public AnimNode< T >, public Source_Connector< T >
{
public:
using AnimNode< T >::instance;
ClientMarker cm;

    Source_AnimNode( Instance * inst, T const & default_value ) :
        AnimNode< T >( inst, default_value ),
        Source_Connector< T >( inst, default_value, this ),
        cm( inst, this, "Source_AnimNode" )
    {
        this->cm.inherits( this->AnimNode< T >::cm, this->Source_Connector< T >::cm );
    }
};

}
