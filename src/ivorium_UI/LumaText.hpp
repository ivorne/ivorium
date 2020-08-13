#pragma once

#include "LumaSystem.hpp"
#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{
    
/**
    \ingroup Elements
    \brief Text with theme-defined font and color.
    
    \ref Text implementation that automatically sets font and color according to current LumaStyle theme.
    This needs to be put into a \ref TextLayout to be usable in a scene graph.
*/
class LumaText : public Text, public LumaListener
{
public:
ClientMarker cm;
    LumaText( Instance * inst, LumaStyleId style_id = LumaStyleId(), bool monospace = false );
    
protected:
    virtual void LumaStyleChanged( LumaStyle const & ) override;
    
private:
    bool monospace;
};

}
