#pragma once

#include "Slot.hpp"
#include "Axis.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief A Slot that can be placed inside a \ref Divider.
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

/**
    \ingroup Elements
    \brief Container that divides its space between children according to their priorities.
    
    Axis along which the space will be divided must be set.
    Children with higher priority (lower order) will get required space before children with lower priority (higher order).
    Each child has two orders - \ref DividerSlot::lowerOrder is used when the slot is smaller than its preferred size and \ref DividerSlot::higherOrder is used to allow child to resize above its preferred size.
    
    Example:
    \code{.cpp}
    auto divider = this->createChild< iv::Divider >()
        ->axis( iv::Axis::X )
        ->axisOrder( iv::AxisOrder::Incremental );
    
    // green square that will have higher priority before it reaches its preferred size
    divider->createChild< iv::DividerSlot >()
        ->lowerOrder( 0 )
        ->higherOrder( 2 )
        ->createChild< iv::Image >()
            ->filename( "/ivorium_graphics/generic/white.png" )
            ->colorTransform( iv::ColorTransform::Scale( 0, 1, 0, 1 ) );
    
    // blue square with lower priority; but once both squares reach their preferred size, they will both resize with the same speed (because their higherOrder is equal)
    divider->createChild< iv::DividerSlot >()
        ->lowerOrder( 1 )
        ->higherOrder( 2 )
        ->createChild< iv::Image >()
            ->filename( "/ivorium_graphics/generic/white.png" )
            ->colorTransform( iv::ColorTransform::Scale( 0, 0, 1, 1 ) );
    \endcode
*/
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
