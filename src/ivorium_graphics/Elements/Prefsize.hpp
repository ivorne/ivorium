#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief Container that can override preferred size of its child.
    
    If a dimension has empty (std::nullopt) prefsize value, then that Prefsize will not override that axis.
*/
class Prefsize : public OneChildElem< SlotChild >, public SlotChild
{
public:
ClientMarker cm;
using Elem::instance;
                                        Prefsize( Instance * );
    
// initialization parameters
    DirtyAttr< std::optional< float > > attr_prefsizeX;
    DirtyAttr< std::optional< float > > attr_prefsizeY;
    DirtyAttr< std::optional< float > > attr_prefsizeZ;
    
    DirtyAttr< float3 >                 attr_prefsizeScale;
    
// utility methods
    // Elem
    Prefsize *                          enabled( bool );
    
    // Prefsize
    Prefsize *                          prefsize( float3 );
    Prefsize *                          prefsizeX( std::optional< float > );
    Prefsize *                          prefsizeY( std::optional< float > );
    Prefsize *                          prefsizeZ( std::optional< float > );
    Prefsize *                          prefsizeScale( float3 ); ///< \brief Multiplies prefsize by this value before assigning it.

protected:
    // Elem
    virtual void                        first_pass_impl( ElementRenderer * ) override;
    virtual void                        second_pass_impl( ElementRenderer * ) override;
};

}
