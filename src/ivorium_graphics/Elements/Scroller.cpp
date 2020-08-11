#include "Scroller.hpp"

#include <ivorium_systems/ivorium_systems.hpp>

namespace iv
{

static constexpr float ScrollStep = 40.0f;
static constexpr float SmoothingFactor = 0.5f;
static constexpr float DragActivationThresholdDistancePx = 5.0f;
static constexpr float SmoothingDeactivationThreshold_Px = 5.0f;

static constexpr float BorderOverreachPx = 10.0f;
static constexpr float DelayUntilFullClamp_frames = 8;

static constexpr float ResidualSpeedInitialFactor = 0.575f;
static constexpr float ResidualSpeedDropOffMultiplier = 0.945f;
static constexpr float ResidualSpeedMinimumThreshold = 0.1f;

Scroller::Scroller( Instance * inst ) :
    OneChildElem< SlotChild >( inst ),
    SlotChild( this ),
    FixedUpdateClient( inst, Defs::Time::Default ),
    FrameUpdateClient( inst ),
    cm( inst, this, "Scroller", ClientMarker::Status() ),
    attr_position( &this->cm, this, 0 ),
    attr_outsize( &this->cm, this, 0 ),
    attr_insize( &this->cm, this, 0 ),
    attr_overlapTop( &this->cm, this, 0 ),
    attr_overlapBottom( &this->cm, this, 0 ),
    position( 0 ),
    smoothedPosition( 0 ),
    ibq( inst ),
    iq( inst ),
    _candidate( std::nullopt ),
    _active( std::nullopt ),
    _candidate_pos( 0 ),
    _residual_speed( 0 ),
    _full_clamp_countdown( 0 )
{
    this->cm.inherits( this->OneChildElem< SlotChild >::cm, this->SlotChild::cm, this->FixedUpdateClient::cm, this->FrameUpdateClient::cm );
    this->cm.owns( this->ibq.cm );
    this->fixed_update_pause();
}

void Scroller::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Scroller" );
}

void Scroller::frame_update()
{
    if( this->_candidate.has_value() )
    {
        auto key = this->_candidate.value();
        float2 pos = this->input_position( key );
        
        float dist = std::abs( pos.y - this->_candidate_pos );
        if( dist > DragActivationThresholdDistancePx )
        {
            this->accept_candidate();
        }
    }
    
    if( this->_active.has_value() )
    {
        this->refresh_clamp_countdown();
        auto key = this->_active.value();
        float2 pos = this->input_position( key );
        
        auto pos_prev = this->position.Get();
        this->position.Set( pos_prev - ( pos.y - this->_candidate_pos ) );
        this->clamp_position();
        //this->_candidate_pos = pos_prev + this->_candidate_pos - this->position.Get();
        this->_candidate_pos = pos.y;
    }
}

void Scroller::fixed_update( TimeId time, int time_step, int steps )
{
    for( size_t i = 0; i < steps; i++ )
    {
        // full clamp countdown update
        if( this->_residual_speed != 0.0f )
            this->refresh_clamp_countdown();
        else if( this->_full_clamp_countdown > 0 )
            this->_full_clamp_countdown--;
            
        // residual speed update
        this->position.Set( this->position.Get() + this->_residual_speed );
        this->clamp_position();
        this->_residual_speed *= ResidualSpeedDropOffMultiplier;
        
        // smooth position update
        float direction = this->position.Get() - this->smoothedPosition.Get();
        this->smoothedPosition.Set( this->smoothedPosition.Get() + direction * SmoothingFactor );
        
        // residual speed termination
        if( std::abs( this->_residual_speed ) < ResidualSpeedMinimumThreshold || std::abs( direction ) < SmoothingDeactivationThreshold_Px )
            this->_residual_speed = 0.0f;
        
        // smooth position termination
        if( this->_residual_speed == 0.0f && this->_full_clamp_countdown <= 0 && std::abs( direction ) < SmoothingDeactivationThreshold_Px )
        {
            this->smoothedPosition.Set( this->position.Get() );
            this->fixed_update_pause();
            break;
        }
    }
}

void Scroller::accept_candidate()
{
    auto key = this->_candidate.value();
    this->_candidate = std::nullopt;
    this->_active = key;

    if( auto root = this->input_getRoot() )
    {
        root->reserve_key( key, true );
        this->reserve_hover_keys( root, true );
        root->treeRefresh();
    }
}

float2 Scroller::input_position( Input::DeviceKey key )
{
    auto input_pos = this->iq.input_position( key.first, key.second );
    return this->FromScreenPlaneToLocalPlane( input_pos );
}


Scroller * Scroller::input_id( InputId id )
{
    this->_input_id = id;
    this->input_treeRefresh();
    return this;
}

InputId Scroller::input_id()
{
    return this->_input_id;
}

void Scroller::input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace )
{
    this->Elem::input_process( root, key, press, real, offspace );
    
    // check binding
    if( !this->ibq.IsBound( this->_input_id, key ) )
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Refuse input (not bound)." );
        return;
    }
    
    // hittest
    int2 pos = this->iq.input_position( key.first, key.second );
    bool hit = this->picking_test( pos );
    float2 input_pos = this->input_position( key );
    
    //
    if( !this->_active.has_value() && !this->_candidate.has_value() && press && real && hit && !offspace )
    {
        this->_candidate = key;
        this->_candidate_pos = input_pos.y;
        this->frame_update_resume();
        this->_residual_speed = 0.0f;
    }
    else if( this->_candidate == key && !press )
    {
        this->_candidate = std::nullopt;
    }
    else if( this->_active == key && !press )
    {
        this->frame_update_pause();
        this->_active = std::nullopt;
        this->reserve_hover_keys( root, false );
        this->input_treeRefresh();
        this->_residual_speed = ( this->position.Get() - this->smoothedPosition.Get() ) * ResidualSpeedInitialFactor;
        if( this->_residual_speed != 0.0f )
            this->fixed_update_resume();
    }
}

void Scroller::refresh_clamp_countdown()
{
    this->_full_clamp_countdown = DelayUntilFullClamp_frames;
    //this->fixed_update_resume();
}

void Scroller::clamp_position()
{
    // determine overreach
    float overreach;
    if( this->_full_clamp_countdown > 0 )
        overreach = BorderOverreachPx;
    else
        overreach = 0.0f;
        
    if( this->child.Get() )
    {
        // fetch dimensions
        auto insize = this->child.Get()->preferredSize.Get().y;
        auto outsize = this->size.Get().y;
        
        // do clamping
        if( insize > outsize )
            this->position.Set( std::clamp( this->position.Get(), 0.0f - overreach, insize - outsize + overreach ) );
        else
            this->position.Set( 0 );
    }
    else
    {
        this->position.Set( 0 );
    }
}

bool Scroller::input_trigger_process( InputRoot * root, Input::DeviceKey key )
{
    if( !this->Elem::input_trigger_process( root, key ) )
        return false;
    
    if( key.first == Input::Key::MouseScrollUp )
    {
        this->refresh_clamp_countdown();
        this->position.Set( this->position.Get() - ScrollStep );
        this->clamp_position();
        return false;
    }
    else if( key.first == Input::Key::MouseScrollDown )
    {
        this->refresh_clamp_countdown();
        this->position.Set( this->position.Get() + ScrollStep );
        this->clamp_position();
        return false;
    }
    
    return true;
}

void Scroller::first_pass_impl( ElementRenderer * er )
{
    if( !this->child.Get() )
    {
        this->preferredSize.Set( float3( 0, 0, 0 ) );
        return;
    }
    
    if( this->child.dirty() || this->expectedSize.dirty() )
    {
        this->expectedSize.clear_dirty();
        this->child.Get()->expectedSize.Set( this->expectedSize.Get() );
    }
    
    this->child.Get()->elem()->first_pass( er );
    
    if( this->child.dirty() || this->child.Get()->preferredSize.dirty() )
    {
        this->preferredSize.Set( this->child.Get()->preferredSize.Get() );
    }
    
    if( this->position.dirty() )
    {
        this->position.clear_dirty();
        if( this->position.Get() != this->smoothedPosition.Get() )
            this->fixed_update_resume();
    }
    
    if( this->child.dirty() || this->child.Get()->preferredSize.dirty() || this->smoothedPosition.dirty() || this->scissor.dirty() )
        er->QueueSecondPass( this );
}

void Scroller::second_pass_impl( ElementRenderer * er )
{
    if( !this->child.Get() )
        return;
    
    if( this->child.dirty() || this->size.dirty() || this->modelTransform.dirty() || this->child.Get()->preferredSize.dirty() || this->smoothedPosition.dirty() || this->scissor.dirty() )
    {
        // clear dirty
        this->child.Get()->preferredSize.clear_dirty();
        this->child.clear_dirty();
        this->size.clear_dirty();
        this->modelTransform.clear_dirty();
        this->smoothedPosition.clear_dirty();
        this->scissor.clear_dirty();
        
        // update visualization attributes
        auto insize = this->child.Get()->preferredSize.Get().y;
        auto outsize = this->size.Get().y;
        auto position_max = ( insize > outsize ) ? ( insize - outsize ) : 0.0f;
        
        this->Attribute_Set( &this->attr_position, this->smoothedPosition.Get() );
        this->Attribute_Set( &this->attr_insize, insize );
        this->Attribute_Set( &this->attr_outsize, outsize );
        this->Attribute_Set( &this->attr_overlapTop, ( this->smoothedPosition.Get() < 0.0f ) ? ( 0.0f - this->smoothedPosition.Get() ) : 0.0f );
        this->Attribute_Set( &this->attr_overlapBottom, ( this->smoothedPosition.Get() > position_max ) ? ( this->smoothedPosition.Get() - position_max ) : 0.0f );
        
        // aggregate size
        float3 new_size = this->size.Get();
        new_size.y = this->child.Get()->preferredSize.Get().y;
        
        // aggregate transform
        this->clamp_position();
        //auto smoothedPosition = this->smoothedPosition.Get();     // this allows physical overreach
        auto smoothedPosition = std::clamp( this->smoothedPosition.Get(), 0.0f, std::max( 0.0f, insize - outsize ) );       // this just shows overreach
        auto model = this->modelTransform.Get() * glm::translate( float4x4( 1 ), float3( 0, 0 - smoothedPosition, 0 ) );
        
        // my scissor
        ShaderScissor my_scissor;
        my_scissor.enabled = true;
        my_scissor.model = this->modelTransform.Get();
        my_scissor.size = this->size.Get();
        
        // set values
        this->child.Get()->size.Set( new_size );
        this->child.Get()->elem()->modelTransform.Set( model );
        this->child.Get()->elem()->scissor.Set( this->scissor.Get() * my_scissor );
        this->child.Get()->elem()->second_pass( er );
    }
}

void Scroller::reserve_hover_keys( InputRoot * root, bool reserve )
{
    // reserve hover keys
    this->ibq.ForeachHoverBinding(
        this->_input_id,
        [ root, reserve ]( Input::Key key, int device_id )
        {
            root->reserve_key( Input::DeviceKey( key, device_id ), reserve );
        }
    );
}

Scroller * Scroller::enabled( bool val )
{
    this->Elem::enabled( val );
    return this;
}

}
