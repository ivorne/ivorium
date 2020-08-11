#pragma once

#include "Elem.hpp"
#include "VectorChildrenElem.hpp"

namespace iv
{

/**
*/
class SlotChild : public Pickable
{
public:
ClientMarker cm;

                            SlotChild( Elem * elem );
    Instance *              instance() const;
    
    void                    status( iv::TableDebugView * view );
    
// post-initialization parameters
    /**
        Do not set this manually, this is computed during down phase of first_pass.
        It can help elements like TextLayout determine which size will be available for them and adjust their preferred size according to it (TextLayout will use width at most of expected_size and height so that all text fits).
        Infinite value means that element will probably take any preferred_size as its size.
        What size will SlotChild probably get if it requests infinite size.
        What is the maximum preferred size that can SlotChild request and still probably get it.
    */
    DirtyAttr< float3 >     expectedSize;
    
// synthesized parameters
    DirtyAttr< float3 >     preferredSize;
    
// inherited parameters
    DirtyAttr< float3 >     size;
    
    // Pickable
    virtual bool            picking_test( int2 input_pos ) override;
    virtual bool            picking_test_pixel_perfect( float2 local_pos ){ return true; }
};


/**
    Slot renders children in order in which they are in stored vector.
*/
class Slot : public VectorChildrenElem< SlotChild >, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;
using SlotChild::elem;
                            Slot( Instance * );
    void                    status( iv::TableDebugView * view );
    
protected:
    // Elem
    virtual void            first_pass_impl( ElementRenderer * ) override;
    virtual void            second_pass_impl( ElementRenderer * ) override;
};


/**
    SlotChild interface from the outside.
    Slot implementation available on the inside.
    Use this for compound elements if you don't want outside code to mess with your internal structure.
    Especially useful for graphical containers that utilize basic containers on the inside.
*/
class ProtectedSlot : public Elem, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;
                            ProtectedSlot( Instance * inst );
    
protected:
    // Elem
    virtual void            first_pass_impl( ElementRenderer * ) override;
    virtual void            second_pass_impl( ElementRenderer * ) override;
    
    virtual void            elem_eachChild( std::function< void( Elem * ) > const & ) override;
    virtual void            elem_childDisconnect( Elem * ) override;
    
    // InputNode
    virtual void            input_eachChild( std::function< void( InputNode * ) > const & f ) override;
    virtual void            input_childDisconnect( InputNode * child ) override;
    
protected:
    Slot root;
};

}
