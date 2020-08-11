#include "PickableSlot.hpp"

#include <ivorium_core/ivorium_core.hpp>
#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

PickableSlot::PickableSlot( Instance * inst ) :
    Slot( inst ),
    Position_InputNode::Tester(),
    cm( inst, this, "PickableSlot" ),
    input( inst, this, false ),
    pickable( nullptr )
{
    this->cm.inherits( this->Slot::cm );
    this->cm.owns( this->input.cm );
    
    this->Add_InputNode( &this->input );
}

void PickableSlot::pickable_elem( Pickable * pickable )
{
    this->pickable = pickable;
}

float2 PickableSlot::input_position_local()
{
    // select pickable
    Pickable * pickable = this;
    if( this->pickable )
        pickable = this->pickable;
    
    //
    if( !pickable->elem()->elem_getRoot() )
        return float2( 0.0, 0.0 );
    
    int2 pos = this->input.input_position_screen();
    return pickable->elem()->FromScreenPlaneToLocalPlane( float2( pos ) );
}

bool PickableSlot::position_test( int2 input_pos )
{
    // select pickable
    Pickable * pickable = this;
    if( this->pickable )
        pickable = this->pickable;
    
    //
    return pickable->picking_test( input_pos );
}

}
