#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief Container that allows us to directly modify model matrix of its child. Usable both in UI compositing and in world views.
    
    Works in two modes, depending on value of \ref Transform::propagateSize:
    
    1. When \ref Transform::propagateSize is set to true, then Transform behaves basically like \ref Slot that can have only one child and it can apply transform to child without either parent or the child really knowing about it. Transform does not step into \ref SlotChild::size negotiation between parent and child node. This mode is useful for UI composition.
    2. When \ref Transform::propagateSize is set to false, then Transform does not even try to fit child into parent container. \ref SlotChild::preferredSize 0 is requested from parent and child gets its requested \ref SlotChild::preferredSize. This mode is useful in world views, when child node does not necessarily need to fit inside parent node.
*/
class Transform : public OneChildElem< SlotChild >, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;

                            Transform( Instance * inst );

    void                    status( iv::TableDebugView * view );
    
    // initialization parameters
    DirtyAttr< float3 >     attr_position;
    DirtyAttr< floatQuat >  attr_rotation;
    DirtyAttr< float3 >     attr_scale;
    DirtyAttr< float4x4 >   attr_deformation;
    
    DirtyAttr< float3 >     attr_outerAnchor;
    DirtyAttr< float3 >     attr_innerAnchor;
    
    /**
        Default is false, which means that transform prefsize is 0 and child will get its preferred size.
            Transform may still have size higher than 0, therefore outer_anchor still has a meaning.
        True means that transform will have prefsize the same as its child prefsize and child will get size of transform.
    */
    DirtyAttr< bool >       attr_propagateSize;
    
    //
    Transform *             position( float3 );
    Transform *             rotation( floatQuat );
    Transform *             scale( float3 );
    Transform *             deformation( float4x4 );
    
    Transform *             outerAnchor( float3 );
    Transform *             innerAnchor( float3 );
    
    Transform *             propagateSize( bool );
    
protected:
    // Elem
    virtual void            first_pass_impl( ElementRenderer * er ) override;
    virtual void            second_pass_impl( ElementRenderer * er ) override;
    
private:
    bool outer_transform_refresh;
    bool inner_transform_refresh;
    float4x4 outer_transform;
    float4x4 inner_transform;
};

}
