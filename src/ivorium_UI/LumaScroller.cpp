#include "LumaScroller.hpp"

#include "Defs.hpp"
#include <ivorium_graphics/ivorium_graphics.hpp>

namespace iv
{

static constexpr float IndicatorWidth_px = 1;

LumaScroller::LumaScroller( Instance * inst ) :
    ProtectedSlot( inst ),
    cm( inst, this, "LumaScroller" ),
    child( &this->cm, this, nullptr ),
    heap( inst, &this->cm ),
    aheap( inst )
{
    this->cm.inherits( this->ProtectedSlot::cm, this->InputNode::cm );
    this->cm.owns( this->aheap.cm );
    
    this->iv::PrivValAttr_Owner::Attribute_SetMode( &this->child, iv::Attribute::ValueMode::ValueModify );
    
    // background
    this->root.
        createChild< iv::Prefsize >()
            ->prefsize( iv::float3( 0, 0, 0 ) )
            ->createChild< iv::Image >()
                ->filename( "/ivorium_graphics/generic/white.png" )
                ->colorTransform( ColorTransform::Scale( iv::float4( 0.278f, 0.278f, 0.278f, 1.0f ) ) );
    
    // overlaps
    auto overlap_top_prefsize = this->root.
        createChild< Border >()
            ->borderLeft( 0 )
            ->borderRight( 0 )
            ->borderTop( 0 )
            ->createChild< iv::Prefsize >()
                ->prefsize( iv::float3( 0, 0, 0 ) );
    
    overlap_top_prefsize->
        createChild< iv::Image >()
            ->filename( "/ivorium_UI/LumaScroller/overlap.png" )
            ->preblend( float4( 0.278, 0.278, 0.278, 1.0 ) );
    
    auto overlap_bot_prefsize = this->root.
        createChild< Border >()
            ->borderLeft( 0 )
            ->borderRight( 0 )
            ->borderBottom( 0 )
            ->createChild< iv::Transform >()
                ->scale( iv::float3( 1, -1, 0 ) )
                ->propagateSize( true )
                ->createChild< iv::Prefsize >()
                    ->prefsize( iv::float3( 0, 0, 0 ) );
    
    overlap_bot_prefsize->
        createChild< iv::Image >()
            ->filename( "/ivorium_UI/LumaScroller/overlap.png" )
            ->preblend( float4( 0.278, 0.278, 0.278, 1.0 ) );
    
    // indicator
    auto indicator_border = this->root.
        createChild< iv::Border >()
            ->borderRight( 0 )
            ->borderTop( 0 )
            ->borderBottom( 0 );
    
    auto img_indicator = indicator_border->
        createChild< iv::Prefsize >()
            ->prefsize( iv::float3( IndicatorWidth_px, 0, 0 ) )
            ->createChild< iv::Image >()
                ->filename( "/ivorium_graphics/generic/white.png" )
                ->colorTransform( ColorTransform::Scale( float4( 0.55, 0.55, 0.55, 1.0 ) ) )
                ->preblend( float4( 0.28, 0.28, 0.28, 1.0 ) );
    
    // scroller
    this->scroller = this->root.
        createChild< iv::Border >()
            ->borderLeft( 0 )
            ->borderRight( IndicatorWidth_px )
            ->borderTop( 0 )
            ->borderBottom( 0 )
            ->createChild< Scroller >()
                ->input_id( Defs::Input::UI_MousePrimary );
    
    //---------- animation ---------------
    // sources
    AnimNode< float > * in_overlap_top = this->aheap.Attribute_SourceNode< float >( &this->scroller->attr_overlapTop, 0.0f )
                                            ->label( "in_overlap_top" );
    AnimNode< float > * in_overlap_bot = this->aheap.Attribute_SourceNode< float >( &this->scroller->attr_overlapBottom, 0.0f )
                                            ->label( "in_overlap_bot" );
    AnimNode< float > * in_position = this->aheap.Attribute_SourceNode< float >( &this->scroller->attr_position, 0.0f )
                                            ->label( "in_position" );
    AnimNode< float > * in_insize = this->aheap.Attribute_SourceNode< float >( &this->scroller->attr_insize, 0.0f )
                                            ->label( "in_insize" );
    AnimNode< float > * in_outsize = this->aheap.Attribute_SourceNode< float >( &this->scroller->attr_outsize, 0.0f )
                                            ->label( "in_outsize" );
    
    // destinations
    AnimNode< std::optional< float > > * out_overlap_top_height = this->aheap.Attribute_DestinationNode( &overlap_top_prefsize->attr_prefsizeY, std::optional< float >() )
                                                                        ->label( "out_overlap_top_height" );
    AnimNode< std::optional< float > > * out_overlap_bot_height = this->aheap.Attribute_DestinationNode( &overlap_bot_prefsize->attr_prefsizeY, std::optional< float >() )
                                                                        ->Speed( 10.0f )
                                                                        ->label( "out_overlap_bot_height" );
    AnimNode< std::optional< float > > * out_indicator_top = this->aheap.Attribute_DestinationNode( &indicator_border->attr_borderTop, std::optional< float >() )
                                                                        ->Speed_inf()
                                                                        ->label( "out_indicator_top" );
    AnimNode< std::optional< float > > * out_indicator_bot = this->aheap.Attribute_DestinationNode( &indicator_border->attr_borderBottom, std::optional< float >() )
                                                                        ->Speed_inf()
                                                                        ->label( "out_indicator_bot" );
    AnimNode< float > * out_indicator_alpha = this->aheap.Attribute_DestinationNode( &img_indicator->shading.alpha, 0.0f )
                                                            ->Speed( 1.0f )
                                                            ->label( "out_indicator_alpha" );
    
    // lambda connector
    Lambda_Connector * lambda = this->aheap.Make_Lambda_Connector()
        ->AddParent( in_overlap_top )
        ->AddParent( in_overlap_bot )
        ->AddParent( in_position )
        ->AddParent( in_insize )
        ->AddParent( in_outsize )
        ->AddChild( out_overlap_top_height )
        ->AddChild( out_overlap_bot_height )
        ->AddChild( out_indicator_top )
        ->AddChild( out_indicator_bot )
        ->AddChild( out_indicator_alpha );
    
    lambda->Retargeting(
        [ in_overlap_top, in_overlap_bot, in_position, in_insize, in_outsize, out_overlap_top_height, out_overlap_bot_height, out_indicator_top, out_indicator_bot, out_indicator_alpha ]( bool warping )
        {
            //std::cout << "LumaScroller: Retarget" << std::endl;
            
            // top overlap
            if( in_overlap_top->Target() > out_overlap_top_height->Target() )   // Imperfect direction detection, but it might work.
                out_overlap_top_height->Speed_inf();
            else
                out_overlap_top_height->Speed( 25.0f );
            
            out_overlap_top_height->Target( in_overlap_top->Target() );
            
            // bottom overlap
            if( in_overlap_bot->Target() > out_overlap_bot_height->Target() )   // Imperfect direction detection, but it might work.
                out_overlap_bot_height->Speed_inf();
            else
                out_overlap_bot_height->Speed( 25.0f );
            out_overlap_bot_height->Target( in_overlap_bot->Target() );
            
            // indicator
            if( in_outsize->Target() >= in_insize->Target() )
            {
                out_indicator_alpha->Target( 0.0f );
                
                out_indicator_top->Target( 0.0f );
                out_indicator_bot->Target( 0.0f );
            }
            else
            {
                out_indicator_alpha->Target( 1.0f );
                
                float outsize = in_outsize->Target();
                float insize = in_insize->Target();
                float pos = std::clamp( in_position->Target(), 0.0f, insize - outsize );
                
                float top = outsize * pos / insize;
                float height = outsize * outsize / insize;
                float bottom = outsize - top - height;
                
                out_indicator_top->Target( top );
                out_indicator_bot->Target( bottom );
            }
        }
    );
}

void LumaScroller::PrivValAttr_Modified( Attribute * )
{
    this->scroller->setChild( this->child.Get() );
}

void LumaScroller::first_pass_impl( ElementRenderer * er )
{
    this->ProtectedSlot::first_pass_impl( er );
}

}
