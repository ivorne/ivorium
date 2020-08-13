#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

/**
    \ingroup Elements
    \brief Vertical scroller with simple graphics.
    
    This class puts visual decorators on top of basic \ref Scroller functionality - mainly slider on right side.
    Use \ref LumaFrame if you need visible borders around the scroller.
*/
class LumaScroller : public ProtectedSlot, protected PrivValAttr_Owner
{
public:
ClientMarker cm;
using ProtectedSlot::instance;

                            LumaScroller( Instance * );
    
// initialization parameters
    PrivValAttr< SlotChild * > child;
    
// utility methods
    template< class T, class ... CArgs >
    T *                     createChild( CArgs const & ... cargs );
    
protected:
    // ProtectedSlot
    virtual void            first_pass_impl( ElementRenderer * ) override;
    
    // PrivValAttr_Owner
    virtual void            PrivValAttr_Modified( Attribute * ) override;
    
private:
    Heap heap;
    AnimHeap aheap;
    Scroller * scroller;
};

/**
*/
template< class T, class ... CArgs >
T * LumaScroller::createChild( CArgs const & ... cargs )
{
    static_assert( std::is_base_of< SlotChild, T >::value, "Child must inherit SlotChild." );
    auto t = this->heap.createClient< T >( cargs ... );
    this->child.Modify( &this->cm, t );
    return t;
}

}
