#pragma once

#include <ivorium_graphics/ivorium_graphics.hpp>
#include "LumaSystem.hpp"

namespace iv
{

/**
    \ingroup Elements
    \brief Flexible base for variously styled buttons that needs to be supplied multiple textures.
    
    At least \ref LumaButton::sufrace should be set.
    \ref LumaButton::hot is nice addition for visual responsiveness and \ref LumaButton::overlay should contain foreground graphics - symbols, glyphs or an icon.
    
*/
class LumaButton : public PickableSlot, public LumaListener, public TranslucentElem
{
public:
ClientMarker cm;
using PickableSlot::instance;
    
    static const constexpr int          DefaultShadowHeightPx = 2;
    
                                        LumaButton( Instance * inst, LumaStyleId style = LumaStyleId() );
    
    LumaButton *                        sufraceNeutralColor( std::optional< float4 > );
    LumaButton *                        overlayNeutralColor( std::optional< float4 > );
    LumaButton *                        resizeStage( FlatShader::ResizeStage );
    LumaButton *                        surface( ResourcePath );
    LumaButton *                        overlay( ResourcePath );
    LumaButton *                        hot( ResourcePath );
    
    LumaButton *                        preblend( float4 val );
    LumaButton *                        translucent( bool val );
    
    // initialization parameters
    /**
    */
    DirtyAttr< std::optional< float4 > >attr_sufraceNeutralColor;
    
    /**
    */
    DirtyAttr< std::optional< float4 > >attr_overlayNeutralColor;
    
    /**
        Resize stage option used for all textures.
    */
    DirtyAttr< FlatShader::ResizeStage > attr_resizeStage;
    
    /**
        Color are rotated from NeutralColor to surface or highlight color.
        Base graphics for the button.
    */
    DirtyAttr< ResourcePath > attr_surface;
    
    /**
        Displayed on top of everything.
        Colors are rotated from NeutralColor to on_surface or on_highlight color.
    */
    DirtyAttr< ResourcePath > attr_overlay;
    
    /**
        Displayed on top of surface during hover and duration.
        Colors are rotated from NeutralColor to surface or highlight color.
    */
    DirtyAttr< ResourcePath > attr_hot;
    
protected:
    // Elem
    virtual void                        first_pass_impl( ElementRenderer * ) override;

    // LumaButton
    /**
        Useful when inheriting LumaButton, adding specifically animated graphics on top of it.
        This tells us about the color shheme currently in use by LumaButton, so that the extra graphics can sync with that.
    */
    virtual void                        ColorsChanged( float4 surface, float4 on_surface ){}
    
    void                                CallColorsChanged();
    Slot                                surface_slot;
    
protected:
    virtual void                        LumaStyleChanged( LumaStyle const & ) override;
    
private:
    AnimHeap anim;
    
    Lambda_Connector *  lambda;
    Activator           activator_ref;
    bool                hot_last;
    
    //Image * img_shadow;
    Image * img_surface;
    Image * img_overlay;
    
    Image * img_activation;
    Image * img_hot;
};

}
