#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
    Does nothing by default.
    Usually is used to give child item its desired size (and usually aspect) and anchor it somewhere in parent container.
    Or can be used to resize child item to whatever size but to keep aspect.
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
