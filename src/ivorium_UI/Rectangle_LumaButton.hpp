#pragma once

#include "LumaButton.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief Simple rectangular button.
    
    Input events are generated in PickableSlot::input (Rectangle_LumaButton indirectly inherits PickableSlot).
*/
class Rectangle_LumaButton : public LumaButton
{
public:
ClientMarker cm;
using LumaButton::instance;
    
                            Rectangle_LumaButton( iv::Instance * inst, ResourcePath const & icon, char const * label, LumaStyleId style_id = LumaStyleId() );
    
protected:
    virtual void            ColorsChanged( float4 surface, float4 on_surface ) override;
    
private:
    Image * img_icon;
    Text * text;
};

}
