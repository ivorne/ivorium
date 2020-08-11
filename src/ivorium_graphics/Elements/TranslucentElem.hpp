#pragma once

#include "Elem.hpp"
#include <ivorium_core/ivorium_core.hpp>

namespace iv
{

/**
    Elements that can be translucent.
    This provides 'preblend' and 'allow_translucent' initialization parameters.
    If you want to handle translucency youself, you don't need to use this, this is just a simple helper class.
    
    By default, items will be rendered without blending (to allow sorting based on opengl state switches and therefore improve performance).
*/
class TranslucentElem
{
public:
ClientMarker cm;

                                TranslucentElem( Elem * elem );
    void                        status( iv::TableDebugView * view );
    
    Elem *                      elem();
    Elem const *                elem() const;
    
    // initialization parameters
    DirtyAttr< float4 >         attr_preblend;          ///< Enables blending in fragment shader - blends into a fixed color rather than into the current content of framebuffer. This allows us to emulate blending in static scenes without requiring specific draw order.
    DirtyAttr< bool >           attr_translucent;       ///< Item will be drawn as translucent - enables blending and draws after all opaque elements sorted by depth with other translucent objects.
    
    // methods
    TranslucentElem *           preblend( float4 val );
    TranslucentElem *           translucent( bool val );
    
private:
    Elem * _elem;
};

}
