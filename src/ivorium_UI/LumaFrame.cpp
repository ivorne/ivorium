#include "LumaFrame.hpp"

namespace iv
{

LumaFrame::LumaFrame( iv::Instance * inst ) :
    iv::ProtectedSlot( inst ),
    cm( inst, this, "LumaFrame" ),
    attr_frame( &this->cm, iv::ResourcePath() ),
    attr_frameWidth( &this->cm, 0.0f ),
    attr_open( &this->cm, true ),
    child( &this->cm, this, nullptr ),
    anim( inst )
{
    this->cm.inherits( this->iv::ProtectedSlot::cm );
    this->cm.owns( this->anim.cm );
    
//----------- visuals ---------------------
    auto outer_prefsize = this->root.
        createChild< iv::Prefsize >();
    
    /*
    auto outer_prefsize = this->root.
        createChild< iv::Border >()
            ->borderLeft( 20 )
            ->borderRight( 20 )
            ->borderTop( 75 )
            ->borderBottom( 75 )
            ->createChild< iv::Align >()
                ->dontExpand( true )
                ->innerAnchor( iv::float3( 0.5f, 0.5f, 0.0f ) )
                ->createChild< iv::Prefsize >()
                    ->prefsizeX( 400 );
    */
    
    auto outer_slot = outer_prefsize->
        createChild< iv::Slot >();
    
    this->border = outer_slot->createChild< iv::Border >();
    
    this->scroller = border->createChild< iv::LumaScroller >();
            
    this->img_frame = outer_slot->createChild< iv::Prefsize >()
        ->prefsize( iv::float3( 0, 0, 0 ) )
        ->createChild< iv::Image >()
            ->resizeStage( iv::FlatShader::ResizeStage::Frame )
            ->translucent( true );
    
    
//----------- animation -------------------
    constexpr float OpenCloseTime_s = 0.3f;
    
    // source
    iv::AnimNode< bool > * in_open = this->anim.Attribute_SourceNode( &this->attr_open, true )
        ->label( "in_open" );
    
    // internal
    iv::AnimNode< iv::float3 > * s_prefsize_scale = this->anim.Node< iv::float3 >( iv::float3( 0, 0, 0 ) )
        ->label( "s_prefsize_scale" );
    
    // destination
    iv::AnimNode< bool > * out_enabled = this->anim.Attribute_DestinationNode( &this->iv::ProtectedSlot::attr_enabled, false )
        ->label( "out_enabled" )
        ->Speed_inf();
    
    iv::AnimNode< iv::float3 > * out_prefsize_scale = this->anim.Attribute_DestinationNode( &outer_prefsize->attr_prefsizeScale, iv::float3( 1, 1, 1 ) )
        ->label( "out_prefsize_scale" )
        ->Speed( 1.0f / OpenCloseTime_s );
    
    // connectors
    this->anim.Make_TimeTransform_Connector( iv::TimeTransform( iv::QuickstepTransform() ), s_prefsize_scale, out_prefsize_scale );
    
    this->connector = this->anim.Make_Lambda_Connector()
        ->AddParent( in_open )
        ->AddChild( out_enabled )
        ->AddChild( s_prefsize_scale );
    
    this->connector->Retargeting(
        [ in_open, s_prefsize_scale, out_enabled ]( bool warping )
        {
            out_enabled->Target( true );
            
            if( in_open->Target() )
                s_prefsize_scale->Target( iv::float3( 1, 1, 1 ) );
            else
                s_prefsize_scale->Target( iv::float3( 1, 0.0001, 1 ) );     ///< Prefsize 0 is kind of a special case for some elements. Not much need to try to fix this yet.
        }
    );
}

void LumaFrame::PrivValAttr_Modified( Attribute * )
{
    this->scroller->child.Modify( &this->cm, this->child.Get() );
}

void LumaFrame::first_pass_impl( iv::ElementRenderer * er )
{
    if( this->attr_frame.clear_dirty() )
    {
        this->img_frame->filename( this->attr_frame.Get() );
    }
    
    if( this->attr_frameWidth.clear_dirty() )
    {
        float width = this->attr_frameWidth.Get();
        this->border->borderLeft( width );
        this->border->borderRight( width );
        this->border->borderTop( width );
        this->border->borderBottom( width );
    }
    
    this->ProtectedSlot::first_pass_impl( er );
}

LumaFrame * LumaFrame::frame( iv::ResourcePath val )
{
    this->attr_frame.Set( val );
    return this;
}

LumaFrame * LumaFrame::frameWidth( float val )
{
    this->attr_frameWidth.Set( val );
    return this;
}

LumaFrame * LumaFrame::open( bool val )
{
    this->attr_open.Set( val );
    return this;
}

}
