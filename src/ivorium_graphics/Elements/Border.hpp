#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief Container that adds margins around child, allowing wide variety of positioning options inside parent.
    
    When some borders values are left unset, then the border stretches automatically to allow child to have its preferred size along that axis.
    
    Typical use cases:
        1. Transparent borders around child - All borders along useful axes are set to 0 or higher value. Child is stretched or shrinked to fill the remaining space inside.
        2. Align and offset child - Leave at least one border unset (std::nullopt) around each useful axis. Child will get its preferred size (if there is enough space) and child will be position with given offset along sides that have border set to a valid value. If neither border along one axis is set, then the child will be centered along that axis.
*/
class Border : public OneChildElem< SlotChild >, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;
                            Border( Instance * );
    
    void                    status( iv::TableDebugView * view );
    
// initialization parameters
    DirtyAttr< std::optional< float > > attr_borderLeft;
    DirtyAttr< std::optional< float > > attr_borderRight;
    
    DirtyAttr< std::optional< float > > attr_borderTop;
    DirtyAttr< std::optional< float > > attr_borderBottom;
    
    DirtyAttr< std::optional< float > > attr_borderFront;
    DirtyAttr< std::optional< float > > attr_borderBack;
    
// utility methods
    Border *                enabled( bool val );

    Border *                borderLeft( std::optional< float > );
    Border *                borderRight( std::optional< float > );
    Border *                borderTop( std::optional< float > );
    Border *                borderBottom( std::optional< float > );
    Border *                borderFront( std::optional< float > );
    Border *                borderBack( std::optional< float > );
    
    Border *                leftTop( std::optional< float > left, std::optional< float > top );
    Border *                leftBottom( std::optional< float > left, std::optional< float > bottom );
    Border *                rightTop( std::optional< float > right, std::optional< float > top );
    Border *                rightBottom( std::optional< float > right, std::optional< float > bottom );
    
    Border *                leftRight( std::optional< float > left, std::optional< float > right );
    Border *                topBottom( std::optional< float > top, std::optional< float > bottom );
    Border *                frontBack( std::optional< float > front, std::optional< float > back );
    
protected:
    // Elem
    virtual void            first_pass_impl( ElementRenderer * ) override;
    virtual void            second_pass_impl( ElementRenderer * ) override;
    
private:
    float3  BorderSums() const;
    void    inner_layout( float3 & out_size, float3 & out_position ) const;
    
private:
    // cache
    bool insizeDirty;
    float3 insize;
    float3 inpos;
};

}
