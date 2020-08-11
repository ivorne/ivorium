#pragma once

#include "Transform_ConnectorI.hpp"
#include "../Animation/AnimNode.hpp"
#include "../../Defs.hpp"
#include <ivorium_core/ivorium_core.hpp>
#include <cmath>

namespace iv
{

/**
    This connector changes value of child node at most at given rate.
    Slows down changes of target - change will not be applied before specified time passes after previous change.
    This consumes changes that are overriden by a following change before the cooldown times out.
    This has two cooldowns - which one is selected depends on if the next value his greater or lesser than the current value.
*/
template< class T >
class Cooldown_Connector : public Transform_ConnectorI< T, T >
{
public:
using Transform_ConnectorI< T, T >::instance;
ClientMarker cm;

//----------------------------- Cooldown_Connector -------------------------------------------------------------------
                                Cooldown_Connector( Instance * inst );
    
    //-------------------------- configuration -----------------------------------------------------
    Cooldown_Connector< T > *   cooldown_increasing( Anim_float );
    Anim_float                  cooldown_increasing();
    
    Cooldown_Connector< T > *   cooldown_decreasing( Anim_float );
    Anim_float                  cooldown_decreasing();
    
//----------------------------- AnimConnector ------------------------------------------------------
    virtual void                UpdatePass_Down() override;
    virtual void                UpdatePass_Up() override;
    
private:
    Anim_float                  _cooldown_increasing;
    Anim_float                  _cooldown_decreasing;
    Anim_float                  _time;
};


}

#include "Cooldown_Connector.inl"
