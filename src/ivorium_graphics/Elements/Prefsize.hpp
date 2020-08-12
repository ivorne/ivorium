#pragma once

#include "Elem.hpp"
#include "Slot.hpp"
#include "OneChildElem.hpp"

namespace iv
{

/**
    \ingroup Elements
    If a dimension has nullopt prefsize value, then that Prefsize will use childs prefsize for that dimension.
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
    
    DirtyAttr< float3 >                 attr_prefsizeScale;     ///< Applies scale on prefsize of all axes. Does not matter if the prefsize is overriden or synthesized.
    
// utility methods
    // Elem
    Prefsize *                          enabled( bool );
    
    // Prefsize
    Prefsize *                          prefsize( float3 );
    Prefsize *                          prefsizeX( std::optional< float > );
    Prefsize *                          prefsizeY( std::optional< float > );
    Prefsize *                          prefsizeZ( std::optional< float > );
    Prefsize *                          prefsizeScale( float3 );

protected:
    // Elem
    virtual void                        first_pass_impl( ElementRenderer * ) override;
    virtual void                        second_pass_impl( ElementRenderer * ) override;
};

}
