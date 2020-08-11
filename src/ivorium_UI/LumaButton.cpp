#include "LumaButton.hpp"

#include <ivorium_graphics/ivorium_graphics.hpp>
#include "Defs.hpp"

namespace iv
{

LumaButton::LumaButton( Instance * inst, LumaStyleId style_id ) :
    PickableSlot( inst ),
    LumaListener( inst, style_id ),
    TranslucentElem( this ),
    cm( inst, this, "LumaButton" ),
    attr_sufraceNeutralColor( &this->cm, std::nullopt ),
    attr_overlayNeutralColor( &this->cm, std::nullopt ),
    attr_resizeStage( &this->cm, FlatShader::ResizeStage::Frame ),
    attr_surface( &this->cm, ResourcePath() ),
    attr_overlay( &this->cm, ResourcePath() ),
    attr_hot( &this->cm, ResourcePath() ),
    surface_slot( inst ),
    anim( inst ),
    hot_last( false )
{
    this->cm.inherits( this->PickableSlot::cm, this->LumaListener::cm, this->TranslucentElem::cm );
    this->cm.owns( this->surface_slot.cm, this->anim.cm );
    
    //
    this->input.input_id( Defs::Input::UI_MousePrimary );
    
    //-------- visuals --------------
    auto surface_border = this->createChild< Border >();
    surface_border->setChild( &this->surface_slot );
        
        this->img_surface = this->surface_slot.createChild< Image >();
        
        auto activation_transform = this->surface_slot.createChild< Transform >()
            ->propagateSize( true );
        
            this->img_activation = activation_transform->createChild< Image >()
                ->colorTransform( ColorTransform::Shift( float4( 1, 1, 1, 1 ) ) * ColorTransform::Zero() )
                ->translucent( true );
        
        this->img_hot = this->surface_slot.createChild< Image >()
            ->translucent( true );
        
        this->img_overlay = this->surface_slot.createChild< Image >();
    
    //
    this->pickable_elem( this->img_surface );
    
    //------- animation -------------
    // in
    AnimNode< bool > * in_enabled = this->anim.Attribute_SourceNode( &this->input.attr_eventEnabled, false )
        ->label( "in_enabled" );
    
    AnimNode< bool > * in_hover = this->anim.Attribute_SourceNode( &this->input.attr_hover, false )
        ->label( "in_hover" );
    
    AnimNode< bool > * in_hot = this->anim.Attribute_SourceNode( &this->input.attr_hot, false )
        ->label( "in_hot" );
    
    AnimNode< Activator > * in_activation = this->anim.Attribute_SourceNode( &this->input.attr_activation, Activator() )
        ->label( "in_activation" );
    
    // 
    AnimNode< bool > * hot_cooldown = this->anim.Node< bool >( false )
        ->label( "hot_cd" );
    
    // out
    AnimNode< float3 > * out_activation_scale = this->anim.Attribute_DestinationNode( &activation_transform->attr_scale, float3( 1, 1, 1 ) )
        ->label( "out_activation_scale" );
    
    AnimNode< float > * out_activation_alpha = this->anim.Attribute_DestinationNode( &this->img_activation->shading.alpha, 0.0f )
        ->label( "out_activation_alpha" );
    
    //
    this->anim.Make_Cooldown_Connector( in_hot, hot_cooldown )
        ->cooldown_increasing( 0.0f )
        ->cooldown_decreasing( 0.15f );
    
    // lambda connector
    this->lambda = this->anim.Make_Lambda_Connector()
        ->AddParent( in_enabled )
        ->AddParent( in_hover )
        ->AddParent( hot_cooldown )
        ->AddParent( in_activation )
        ->AddChild( out_activation_scale )
        ->AddChild( out_activation_alpha );
    
    this->lambda->Retargeting(
        [ = ]( bool warping )
        {
            LumaStyle const & style = this->style();
            
            //-------------- surface color (hover) --------------------------
            float4 surface_color;
            float4 text_color;
            if( !in_enabled->Target() )
            { // disabled
                surface_color = style.surface;
                text_color = style.on_surface;
            }
            else if( in_hover->Target() )
            { // hover
                surface_color = ColorTransform::Apply( ColorTransform::ScaleValue( 1.25f ) * ColorTransform::ShiftSaturation( -0.1f ), style.highlight );
                text_color = style.on_highlight;
            }
            else
            { // normal
                surface_color = style.highlight;
                text_color = style.on_highlight;
            }
            
            // apply colors
            if( this->attr_sufraceNeutralColor.Get().has_value() )
                this->img_surface->colorTransform( ColorTransform::Change( this->attr_sufraceNeutralColor.Get().value(), surface_color ) );
            if( this->attr_overlayNeutralColor.Get().has_value() )
                this->img_overlay->colorTransform( ColorTransform::Change( this->attr_overlayNeutralColor.Get().value(), text_color ) );
            
            // apply colors of inheriting instance
            this->ColorsChanged( surface_color, text_color );
            
            //-------- hot -------------
            bool up = !in_enabled->Target() || !hot_cooldown->Target();
            if( up )
            {
                // hot
                this->img_hot->alpha( 0.0f );
            }
            else
            {
                // hot
                this->img_hot->alpha( 1.0f );
            }
            
            //-------------- activation animation ---------------------------------------
            bool activation_connected = this->input.field_press.AssignmentState() != FieldI::Assignment::Unassigned
                                     || this->input.field_release.AssignmentState() != FieldI::Assignment::Unassigned;
            
            bool hot_start = !this->hot_last && in_hot->Target();
            this->hot_last = in_hot->Target();
            
            if( activation_connected && ( hot_start || ( in_activation->Target().CopyActivations( this->activator_ref ) && out_activation_alpha->Target() != Anim_float( 0.04f ) ) ) )
            { // reset
                out_activation_scale->Target( float3( 0, 0, 0 ) )
                                 ->Speed_inf();
                out_activation_alpha->Target( 0.04f )
                                 ->Speed_inf();
            }
            else if( out_activation_scale->IsInTarget( float3( 0, 0, 0 ) ) )
            { // scale up
                out_activation_scale->Target( float3( 1, 1, 1 ) )
                                 ->Speed( 4.0f );
            }
            else if( out_activation_scale->IsInTarget( float3( 1, 1, 1 ) ) && !in_hot->Target() )
            { // fade out
                out_activation_alpha->Target( 0.0f )
                                 ->Speed( 0.3f );
            }
        }
    );
}

void LumaButton::first_pass_impl( ElementRenderer * er )
{
    if( this->attr_translucent.clear_dirty() )
    {
        auto tr = this->attr_translucent.Get();
        this->img_surface->translucent( tr );
        this->img_overlay->translucent( tr );
    }
    
    if( this->attr_preblend.clear_dirty() )
    {
        auto pr = this->attr_preblend.Get();
        this->img_surface->preblend( pr );
        this->img_overlay->preblend( pr );
    }
    
    // neutralColor
    if( this->attr_sufraceNeutralColor.clear_dirty() )
    {
        this->lambda->Retarget();
    }
    
    if( this->attr_overlayNeutralColor.clear_dirty() )
    {
        this->lambda->Retarget();
    }
    
    // resizeStage
    if( this->attr_resizeStage.clear_dirty() )
    {
        auto rs = this->attr_resizeStage.Get();
        
        this->img_surface->resizeStage( rs );
        this->img_overlay->resizeStage( rs );
        this->img_activation->resizeStage( rs );
        this->img_hot->resizeStage( rs );
    }
    
    // surface
    if( this->attr_surface.clear_dirty() )
    {
        this->img_surface->filename( this->attr_surface.Get() );
        this->img_activation->filename( this->attr_surface.Get() );
    }
    
    // overlay
    if( this->attr_overlay.clear_dirty() )
        this->img_overlay->filename( this->attr_overlay.Get() );
    
    // hot
    if( this->attr_hot.clear_dirty() )
        this->img_hot->filename( this->attr_hot.Get() );
    
    //
    this->PickableSlot::first_pass_impl( er );
}

void LumaButton::CallColorsChanged()
{
    this->lambda->Retarget();
}

void LumaButton::LumaStyleChanged( LumaStyle const & stye )
{
    this->lambda->Retarget();
}

LumaButton * LumaButton::sufraceNeutralColor( std::optional< float4 > val )
{
    this->attr_sufraceNeutralColor.Set( val );
    return this;
}

LumaButton * LumaButton::overlayNeutralColor( std::optional< float4 > val )
{
    this->attr_overlayNeutralColor.Set( val );
    return this;
}

LumaButton * LumaButton::resizeStage( FlatShader::ResizeStage val )
{
    this->attr_resizeStage.Set( val );
    return this;
}

LumaButton * LumaButton::surface( ResourcePath val )
{
    this->attr_surface.Set( val );
    return this;
}

LumaButton * LumaButton::overlay( ResourcePath val )
{
    this->attr_overlay.Set( val );
    return this;
}

LumaButton * LumaButton::hot( ResourcePath val )
{
    this->attr_hot.Set( val );
    return this;
}

LumaButton * LumaButton::preblend( float4 val )
{
    this->TranslucentElem::preblend( val );
    return this;
}

LumaButton * LumaButton::translucent( bool val )
{
    this->TranslucentElem::translucent( val );
    return this;
}

}
