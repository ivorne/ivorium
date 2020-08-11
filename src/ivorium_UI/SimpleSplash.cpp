#include "SimpleSplash.hpp"

namespace iv
{

SimpleSplash::SimpleSplash( Instance * inst ) :
    Slot( inst ),
    DelayedLoader( inst ),
    cm( inst, this, "SimpleSplash" ),
    anim( inst )
{
    this->cm.inherits( this->Slot::cm, this->DelayedLoader::cm );
    this->cm.owns( this->anim.cm );
    
    auto background = this->createChild< Image >()
        ->filename( "/ivorium_UI/SimpleSplash/background.png" )
        ->resizeStage( FlatShader::ResizeStage::Frame )
        ->translucent( true );
    
    auto logo = this->createChild< Border >()
        ->createChild< Image >()
            ->filename( "/ivorium_UI/SimpleSplash/logo.png" )
            ->resizeStage( FlatShader::ResizeStage::Fixed )
            ->translucent( true );
    
    //------ anim ---------
    this->in_enabled = this->anim.Node( false )
        ->label( "in_enabled" );
    
    AnimNode< float > * out_background_alpha = this->anim.Attribute_DestinationNode( &background->shading.alpha, 0.0f )
        ->label( "out_background_alpha" );
    
    AnimNode< float > * out_logo_alpha = this->anim.Attribute_DestinationNode( &logo->shading.alpha, 0.0f )
        ->label( "out_logo_alpha" );
    
    AnimNode< float > * state_timeout= this->anim.State_DestinationNode( 0.0f )
        ->label( "state_timeout" );
    
    this->lambda = this->anim.Make_Lambda_Connector()
        ->AddParent( this->in_enabled )
        ->AddChild( out_background_alpha )
        ->AddChild( out_logo_alpha )
        ->AddChild( state_timeout );
    
    this->lambda->Retargeting(
        [ = ]( bool warping )
        {
            //LumaStyle const & style = this->style();
            
            if( this->in_enabled->Target() )
            {
                out_logo_alpha->Target( 1.0 )
                    ->Speed_inf();
                out_background_alpha->Target( 1.0 )
                    ->Speed_inf();
                
                state_timeout->Target( 0.7f )
                    ->Speed( 1.0f );
            }
            else if( state_timeout->IsInTarget() )
            {
                state_timeout->Target( 0.0f )
                    ->Speed_inf();
                
                out_logo_alpha->Target( 0.0 )
                    ->Speed( 2.0 );
                
                if( out_logo_alpha->IsInTarget( 0.0 ) )
                    out_background_alpha->Target( 0.0 )
                        ->Speed( 3.0 );
            }
        }
    );
    
    //-----------------------
    if( this->LoadingRunning() )
        this->LoadStart();
}

void SimpleSplash::LoadStart()
{
    this->in_enabled->Target( true );
}

void SimpleSplash::LoadFinish()
{
    this->in_enabled->Target( false );
}

}
