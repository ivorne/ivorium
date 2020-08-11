#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
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
