#include "InputEvent.hpp"

#include "../Defs.hpp"

namespace iv
{

InputEvent::InputEvent( Instance * inst ) :
    cm( inst, this, "InputEvent", ClientMarker::Status() ),
    attr_eventEnabled( &this->cm, this, false ),
    attr_activation( &this->cm, this, Activator() ),
    attr_hot( &this->cm, this, false ),
    field_press( inst, this ),
    field_release( inst, this ),
    field_duration( inst, this ),
    inst( inst ),
    release_hot( false ),
    duration( false )
{
    this->cm.owns( this->field_press.cm, this->field_release.cm, this->field_duration.cm );
    
    this->Attribute_SetMode( &this->attr_eventEnabled, Attribute::ValueMode::Value );
    this->Attribute_SetMode( &this->attr_activation, Attribute::ValueMode::Value );
    this->Attribute_SetMode( &this->attr_hot, Attribute::ValueMode::Value );
}

InputEvent::~InputEvent()
{
}

Instance * InputEvent::instance() const
{
    return this->inst;
}

void InputEvent::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "InputEvent" );
    
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "attr_inputEnabled", this->attr_eventEnabled.Get() );
    row.Column( "attr_activation", this->attr_activation.Get() );
    row.Column( "attr_hot", this->attr_hot.Get() );
    
    row.Column( "field_press", this->field_press.Get() )
        .Hint( "assignment", this->field_press.AssignmentState() )
        .Hint( "mode", this->field_press.Mode() );
    row.Column( "field_release", this->field_release.Get() )
        .Hint( "assignment", this->field_release.AssignmentState() )
        .Hint( "mode", this->field_release.Mode() );
    row.Column( "field_duration", this->field_duration.Get() )
        .Hint( "assignment", this->field_duration.AssignmentState() )
        .Hint( "mode", this->field_duration.Mode() );
}

void InputEvent::check_consistency()
{
    if( this->cm.log_enabled( iv::Defs::Log::ConsistencyChecks ) )
    {
        bool disconnected = this->field_press.AssignmentState() == FieldI::Assignment::Unassigned
                         && this->field_release.AssignmentState() == FieldI::Assignment::Unassigned
                         && this->field_duration.AssignmentState() == FieldI::Assignment::Unassigned;
        if( disconnected )
            this->cm.log( SRC_INFO, iv::Defs::Log::ConsistencyChecks, "Fields disconnected." );
    }
}

void InputEvent::ie_start_duration()
{
    if( this->duration )
        return;
    this->duration = true;
    
    this->check_consistency();
    
    if( this->field_duration.Mode() == Attribute::ValueMode::ValueModify )
    {
        this->cm.log( SRC_INFO, Defs::Log::InputEvent, "Start duration." );
        this->PrivField_Owner< SumAgg< int > >::Field_Modify( &this->field_duration, this->field_duration.Get().Insert( 1 ) );
    }
}

void InputEvent::ie_stop_duration()
{
    if( !this->duration )
        return;
    this->duration = false;
    
    this->check_consistency();
    
    if( this->field_duration.Mode() == Attribute::ValueMode::ValueModify )
    {
        this->cm.log( SRC_INFO, Defs::Log::InputEvent, "Stop duration." );
        this->PrivField_Owner< SumAgg< int > >::Field_Modify( &this->field_duration, this->field_duration.Get().Remove( 1 ) );
    }
}

void InputEvent::ie_trigger()
{
    this->check_consistency();
    
    if( this->field_press.Mode() == Attribute::ValueMode::ValueModify || this->field_release.Mode() == Attribute::ValueMode::ValueModify )
        this->cm.log( SRC_INFO, Defs::Log::InputEvent, "Trigger." );
    
    // press
    this->PrivField_Owner< Activator >::Field_Modify( &this->field_press, this->field_press.Get().MakeActivated() );
    
    // release
    this->PrivField_Owner< Activator >::Field_Modify( &this->field_release, this->field_release.Get().MakeActivated() );
}

void InputEvent::ie_start_activation()
{
    this->check_consistency();
    
    if( this->field_press.Mode() == Attribute::ValueMode::ValueModify )
        this->cm.log( SRC_INFO, Defs::Log::InputEvent, "Press activated." );
    
    // press activation
    this->PrivField_Owner< Activator >::Field_Modify( &this->field_press, this->field_press.Get().MakeActivated() );
    
    // release is hot
    if( this->field_release.Mode() == Attribute::ValueMode::ValueModify )
    {
        this->release_hot = true;
        this->Field_OnChanged( (Field< SumAgg< int > > *)nullptr, false );
    }
}

void InputEvent::ie_stop_activation( bool real )
{
    this->check_consistency();
    
    // release activation
    if( real )
    {
        if( this->field_release.Mode() == Attribute::ValueMode::ValueModify )
            this->cm.log( SRC_INFO, Defs::Log::InputEvent, "Release activated." );
        this->PrivField_Owner< Activator >::Field_Modify( &this->field_release, this->field_release.Get().MakeActivated() );
    }
    
    // release is no longer hot
    this->release_hot = false;
    this->Field_OnChanged( (Field< SumAgg< int > > *)nullptr, false );
}

void InputEvent::refresh_enabled()
{
    bool enabled = this->field_press.Mode() == Attribute::ValueMode::ValueModify
                || this->field_release.Mode() == Attribute::ValueMode::ValueModify
                || this->field_duration.Mode() == Attribute::ValueMode::ValueModify;
    
    this->Attribute_Set( &this->attr_eventEnabled, enabled );
}

void InputEvent::Field_OnChanged( Field< Activator > * field, bool real )
{
    // refresh enabled field
    this->refresh_enabled();
    
    // copy activations
    unsigned char activations;
    if( field == &this->field_press )
        activations = this->field_press.Get().CopyActivations( this->press_reference );
    else if( field == &this->field_release )
        activations = this->field_release.Get().CopyActivations( this->release_reference );
    
    // activate visualization
    if( real )
    {
        Activator current = this->attr_activation.Get();
        this->Attribute_Set( &this->attr_activation, Activator( current.Value() + activations ) );
    }
}

void InputEvent::Field_OnChanged( Field< SumAgg< int > > * field, bool real )
{
    // refresh enabled field
    this->refresh_enabled();
    
    // refresh attr_hot
    this->Attribute_Set( &this->attr_hot, this->field_duration.Get().Aggregated() > 0 || this->release_hot );
}

}
