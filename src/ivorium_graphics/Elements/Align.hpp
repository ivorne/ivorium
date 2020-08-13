#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief Container providing special behaviors related to relative alignment of its child.
    
    Use of \ref Align::Border is encouraged for simple positioning tasks over Align (not because performance, but because it covers more usual use cases).
    
    Typical core use cases:
      1. Simple alignment - \ref Align::dontExpand is set to true - Child retains its preferred size if possible and is positioned relatively inside according to \ref Align::innerAnchor size.
      2. Keep aspect ratio - \ref Align::keepAspect is set to true - Child can be shrinked or expanded according to size of its parent, but it will always retain its aspect ration by reducing its size along some axes. \ref Align::innerAnchor determines relative positioning along those reduced axes.
      3. Simple alignment and keep aspect ratio - Both \ref Align::dontExpand and \ref Align::keepAspect are set to true - Child does not expand above its preferred size and when it shrinks due to small parent container, it retains its preferred size.
      4. Scale child instead of resizing - \ref Align::resizeScales is set to true - Child always gets its preferred size (on the inside), but is then scaled in order to fit into available space. This can be combined with the above use cases but it can also be used on its own.
*/
class Align : public OneChildElem< SlotChild >, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;
                            Align( Instance * );
                            
    void                    status( iv::TableDebugView * view );
    
// initialization parameters
    DirtyAttr< bool >       attr_dontExpand;
    DirtyAttr< bool >       attr_keepAspect;
    DirtyAttr< bool >       attr_resizeScales;
    DirtyAttr< float3 >     attr_innerAnchor;
    
// utility methods
    // Elem
    Align *                 enabled( bool );
    
    // Align
    Align *                 dontExpand( bool );
    Align *                 keepAspect( bool );
    Align *                 resizeScales( bool );
    Align *                 innerAnchor( float3 );
    
protected:
    // Elem
    virtual void            first_pass_impl( ElementRenderer * ) override;
    virtual void            second_pass_impl( ElementRenderer * ) override;
    
private:
    void inner_layout( float3 & out_size, float3 & out_position, float3 & out_scale ) const;
    
private:
    // cache
    float3 insize;
    float3 inpos;
    float3 inscale;
};

}
