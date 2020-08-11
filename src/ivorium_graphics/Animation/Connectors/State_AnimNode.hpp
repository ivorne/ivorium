#pragma once

#include "Destination_Connector.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Connector that acts as single variable of given time.
    It does not have any outside effects, it only serves for state control in Lambda_Connector and such.
    
    Either use just AnimNode< T >::SetTarget and AnimNode< T >::GetTarget to use it as simple variable.
    Or use also AnimNode< T >::speed, AnimNodeI::WarpToTarget and AnimNode< T >::IsInTarget to use timing in this (possibly use it as a delay node).
    
    If only SetTarget and GetTarget are to be used, it might be better to create just empty AnimNode< T > with no connector attached to it - no ivorium::Interpolator specialization is needed in that case.
*/
template< class T >
class State_AnimNode : public AnimNode< T >, public Destination_Connector< T >
{
public:
using AnimNode< T >::instance;
ClientMarker cm;
    
    State_AnimNode( Instance * inst, T const & initial ) :
        AnimNode< T >( inst, initial ),
        Destination_Connector< T >( inst, this, &this->attr ),
        cm( inst, this, "State_AnimNode" ),
        attr( &this->cm, initial )
    {
        this->cm.inherits( this->AnimNode< T >::cm, this->Destination_Connector< T >::cm );
    }
    
private:
    SharedAttr< T > attr;
};

}
