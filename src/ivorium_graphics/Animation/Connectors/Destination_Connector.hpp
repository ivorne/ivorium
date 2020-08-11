#pragma once

#include "../Animation/AnimNode.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
*/
template< class T, class Interp >
class Destination_ConnectorGI : public AnimConnector
{
public:
using AnimConnector::instance;
ClientMarker cm;
                            Destination_ConnectorGI( Instance * inst, AnimNode< T > * parent = nullptr, Attr< T > * destination = nullptr );
                            ~Destination_ConnectorGI();
    
    void                    SetParent( AnimNode< T > * );
    void                    SetDestination( Attr< T > * );
    
    //--------------------- AnimConnector ------------------------------------------------------
    virtual void            anim_eachParent( std::function< void( AnimNodeI * ) > const & ) override;
    virtual void            anim_parentDisconnect( AnimNodeI * node ) override;
    
    virtual void            UpdatePass_Down() override;
    virtual void            UpdatePass_Up() override;

private:
    Interpolator_float      _accumulated_distance;
    AnimNode< T > *         _parent;
    Attr< T > *             _destination;
    Anim_float              _distance;
};

template< class T >
class Destination_Connector : public Destination_ConnectorGI< T, iv::Interpolator< T > >
{
public:
    using Destination_ConnectorGI< T, iv::Interpolator< T > >::Destination_ConnectorGI;
};

}

#include "Destination_Connector.inl"
