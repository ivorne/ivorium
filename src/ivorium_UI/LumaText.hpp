#pragma once

#include "LumaSystem.hpp"
#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

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
