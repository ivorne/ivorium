#include "DefaultLumaStyles.hpp"

namespace iv
{

DefaultLumaStyles::DefaultLumaStyles( Instance * inst ) :
    LumaStyler( inst ),
    cm( inst, this, "DefaultLumaStyles" )
{
    this->cm.inherits( this->LumaStyler::cm );
}

void DefaultLumaStyles::Light( LumaStyleId core, LumaStyleId secondary )
{
    LumaStyle style;
    style.surface =         float4( 1, 1, 1, 1 );
    style.error =           float4( 0.69, 0, 0.13, 1 );
    style.highlight =       float4( 0.384, 0, 0.93, 1 );
    style.on_surface =      float4( 0, 0, 0, 1 );
    style.on_error =        float4( 1, 1, 1, 1 );
    style.on_highlight =    float4( 1, 1, 1, 1 );
    
    this->style( core, style );
    
    if( secondary.valid() )
    {
        style.highlight = float4( 0.215686275f, 0.0f, 0.701960784f, 1.0f );
        this->style( secondary, style );
    }
}

void DefaultLumaStyles::Green( LumaStyleId core )
{
    LumaStyle style;
    style.surface =         float4( 1, 1, 1, 1 );
    style.error =           float4( 0.69, 0, 0.13, 1 );
    style.highlight =       float4( 0.231, 0.387, 0, 1 );
    style.on_surface =      float4( 0, 0, 0, 1 );
    style.on_error =        float4( 1, 1, 1, 1 );
    style.on_highlight =    float4( 1, 1, 1, 1 );
    
    this->style( core, style );
}

void DefaultLumaStyles::Dark( LumaStyleId core, LumaStyleId secondary )
{
    LumaStyle style;
    style.surface =         float4( 0.116, 0.116, 0.116, 1 );
    style.error =           float4( 0.811, 0.401, 0.474, 1 );
    style.highlight =       float4( 0.735, 0.524, 0.990, 1 );
    style.on_surface =      float4( 1.000, 1.000, 1.000, 1 );
    style.on_error =        float4( 0.000, 0.000, 0.000, 1 );
    style.on_highlight =    float4( 0.000, 0.000, 0.000, 1 );
    
    this->style( core, style );
    
    if( secondary.valid() )
    {
        style.highlight = float4( 0.170, 0.854, 0.776, 1 );
        this->style( secondary, style );
    }
}

}
