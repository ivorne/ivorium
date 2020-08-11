#pragma once

#include "Slot.hpp"
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

/**
    Probably rename this to Pickable_InputNode.
*/
class PickableSlot : public Slot, public Position_InputNode::Tester
{
public:
ClientMarker cm;
using Slot::instance;

                            PickableSlot( Instance * inst );
    
    void                    pickable_elem( Pickable * pickable );
    float2                  input_position_local();
    
    Position_InputNode input;
    
protected:
    // Position_InputNode
    virtual bool            position_test( int2 input_pos ) override;
    
protected:
    Pickable * pickable;
    
};

}
