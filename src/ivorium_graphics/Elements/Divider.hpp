#pragma once

#include "Slot.hpp"
#include "Axis.hpp"

namespace iv
{

class Divider;
/**
    \ingroup Elements
*/
class DividerSlot : public Slot
{
public:
ClientMarker cm;
                            DividerSlot( Instance * inst );
    
    void                    status( iv::TableDebugView * view );
    
// initialization parameters
    /**
     * default: 0
     * It is the order in which will this item gets space.
     * Lower value means that this slot will get space earlier when enlarging Divider and this divider slot has lower size than its preferred size.
    */
    DirtyAttr< unsigned >   attr_lowerOrder;
    
    /**
     * default: 0
     * This is similar to DividerSlot::lower_priority but it applies when DividerSlot gets larger than its preferred size.
    */
    DirtyAttr< unsigned >   attr_higherOrder;
    
// utility methods
    DividerSlot *           lowerOrder( unsigned );
    DividerSlot *           higherOrder( unsigned );
};


class Divider : public VectorChildrenElem< DividerSlot >, public SlotChild
{
public:
using Elem::instance;
ClientMarker cm;

                                    Divider( Instance * inst );
    
    void                            status( iv::TableDebugView * view );
    
// initialization parameters
    DirtyAttr< Axis >               attr_axis;
    DirtyAttr< AxisOrder >          attr_axisOrder;
    
// utility methods
    Divider *                       axis( Axis );
    Divider *                       axisOrder( AxisOrder );
    
protected:
    // Elem
    virtual void                    first_pass_impl( ElementRenderer * ) override;
    virtual void                    second_pass_impl( ElementRenderer * ) override;
    
private:
    struct Phase
    {
        // primary (filled when constructing phases)
        size_t  segment;
        bool    first;      ///< First phase for given child (resets its size).
        bool    higher;
        
        // secondary (filled when updating sizes and weights)
        float   weight;     ///< Ratio of size to give to the item.
        float   size;       ///< Max size to give to the item.
    };
    
    std::multimap< unsigned, Phase > phases;
    
private:
    bool offsets_dirty;
};

}
