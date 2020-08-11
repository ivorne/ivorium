#pragma once

#include "LumaSystem.hpp"

namespace iv
{

/**
    Has multiple methods where each of them sets different color scheme to given style ids.
*/
class DefaultLumaStyles : public LumaStyler
{
public:
ClientMarker cm;
                DefaultLumaStyles( Instance * inst );
    
    /**
        \p core         Default color scheme for the style.
        \p secondary    Color scheme with alternative highlight color.
    */
    void        Light( LumaStyleId primary, LumaStyleId secondary = LumaStyleId() );
    
    /**
    */
    void        Green( LumaStyleId primary );
    
    /**
        \p core         Default color scheme for the style.
        \p secondary    Color scheme with alternative highlight color.
    */
    void        Dark( LumaStyleId primary, LumaStyleId secondary = LumaStyleId() );
};


};
