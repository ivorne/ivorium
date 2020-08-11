#include "Position_InputNode.hpp"
#include "../Defs.hpp"

namespace iv
{

Position_InputNode::Position_InputNode( Instance * inst, Tester * tester, bool keeps_press_in_offspace ) :
    InputNode( inst ),
    InputEvent( inst ),
    FrameUpdateClient( inst ),
    cm( inst, this, "Position_InputNode", ClientMarker::Status() ),
    attr_hover( &this->cm, this, false ),
    tester( tester ),
    keeps_press_in_offspace( keeps_press_in_offspace ),
    ibq( inst ),
    iq( inst ),
    _input_id(),
    _fallthrough( false ),
    _active( std::nullopt ),
    _durated( std::nullopt ),
    _watched(),
    _last( std::nullopt )
{
    this->cm.inherits( this->InputNode::cm, this->InputEvent::cm, this->FrameUpdateClient::cm );
    this->cm.owns( this->ibq.cm );
    
    this->Attribute_SetMode( &this->attr_hover, Attribute::ValueMode::Value );
    
    this->setup_frame_update();
}

Position_InputNode::~Position_InputNode()
{
}

void Position_InputNode::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Position_InputNode" );
    
    auto row = view->Table( DebugTable ).Row( this );
    (void)row;
    
    row.Column( "input_id", this->input_id() );
    row.Column( "fallthrough_enabled", this->fallthrough_enabled() );
    row.Column( "keeps_press_in_offspace", this->keeps_press_in_offspace );
    row.Column( "active", this->_active );
    row.Column( "last", this->_last );
}

void Position_InputNode::input_id( InputId val )
{
    this->_input_id = val;
    this->input_treeRefresh();
}

InputId Position_InputNode::input_id()
{
    return this->_input_id;
}

void Position_InputNode::fallthrough_enabled( bool val )
{
    this->_fallthrough = val;
    this->input_treeRefresh();
}

bool Position_InputNode::fallthrough_enabled()
{
    return this->_fallthrough;
}

void Position_InputNode::setup_frame_update()
{
    if( this->_watched.size() )
        this->frame_update_resume();
    else
        this->frame_update_pause();
}

void Position_InputNode::frame_update()
{
    for( auto & p_devkey : this->_watched )
    {
        // test hit
        int2 input_position = this->iq.input_position( p_devkey.first.first, p_devkey.first.second );
        bool hit = this->tester->position_test( input_position );
        
        // change?
        if( hit != p_devkey.second )
        {
            this->input_treeRefresh();
            break;
        }
    }
}

int2 Position_InputNode::input_position_screen()
{
    if( !this->_last.has_value() )
    {
        Input::Key bound_key = Input::Key::None;
        int bound_device_id = -1;
        
        this->ibq.ForeachBinding(
            this->_input_id,
            [ & ]( Input::Key key, int device_id )
            {
                bound_key = key;
                bound_device_id = device_id;
            }
        );
        
        if( bound_key == Input::Key::None )
            return int2( -1, -1 );
        
        return this->iq.input_position( bound_key, bound_device_id );
    }
    
    Input::DeviceKey key = this->_last.value();
    return this->iq.input_position( key.first, key.second );
}


bool Position_InputNode::input_trigger_process( InputRoot * root, Input::DeviceKey key )
{
    if( !this->ibq.IsBound( this->_input_id, key ) )
        return true;
    
    int2 pos = this->iq.input_position( key.first, key.second );
    bool hit = this->tester->position_test( pos );
    if( !hit )
        return true;
    
    this->ie_trigger();
    return false;
}

void Position_InputNode::input_process_hover( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace )
{
    if( !this->ibq.IsHoverBound( this->_input_id, key ) )
        return;
    
    //
    int2 pos = this->iq.input_position( key.first, key.second );
    bool hit = this->tester->position_test( pos );
    
    // watched
    if( press )
        this->_watched[ key ] = hit;
    else
        this->_watched.erase( key );
    
    this->setup_frame_update();
    
    // change hover
    bool new_hover = press && hit && ( !offspace || ( this->keeps_press_in_offspace && this->_active.has_value() ) );
    //cout << "PositionInputEvent (" << this << ") hover -> " << new_hover << endl;
    this->Attribute_Set( &this->attr_hover, new_hover );
    
    if( hit )
        offspace = true;
    
    // hover is offspaced if we have active thing pressed in offspace (and keeps_press_in_offspace is true)
    //if( this->keeps_press_in_offspace && this->_active.has_value() )  // PROBLEM - we need to reserve the hover
    //  offspace = true;
}

void Position_InputNode::reserve_hover_keys( InputRoot * root, bool reserve )
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

void Position_InputNode::input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace )
{
    // hover input
    this->input_process_hover( root, key, press, real, offspace );
    
    //
    int2 pos = this->iq.input_position( key.first, key.second );
    bool hit = this->tester->position_test( pos );
    
    if( !this->ibq.IsBound( this->_input_id, key ) )
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Refuse input (not bound)." );
        
        // Each position input event will block all input and binding is only to know which should be accepted (fallthrough works only on keys that are bound to attached inputId and only if enabled) .
        if( hit )
            offspace = true;
        return;
    }
    
    //cout << "PositionInputEvent::input_process key="<<ivorium::StringIO_Write( key.first, this->object() )<<" hit="<<hit<<" press="<<press<<" real="<<real<<" offspace="<<offspace<<" queue_refresh="<<queue_refresh << endl;
    
    // watched
    if( press )
        this->_watched[ key ] = hit;
    else
        this->_watched.erase( key );
    this->setup_frame_update();
    
    // duration
    if( press && hit && !offspace )
    {
        if( ( !this->_durated.has_value() || this->_durated == key ) )
        {
            this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input / begin Duration." );
            this->_durated = key;
            this->ie_start_duration();
        }
        else
        {
            this->cm.log( SRC_INFO, Defs::Log::Input, "Reject input / Duration is already active with different key." );
        }
    }
    else
    {
        if( this->_durated == key )
        {
            this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input / end Duration -> queue refresh." );
            
            this->_durated = std::nullopt;
            this->input_treeRefresh();
            this->ie_stop_duration();
        }
        else
        {
            this->cm.log( SRC_INFO, Defs::Log::Input, "Reject input / Duration is active with different key." );
        }
    }
    
    // activation
    if( !this->_active.has_value() && press && real && hit && !offspace )
    { // clicked on this
        offspace = true;
        this->_active = key;
        this->_last = key;
        
        if( this->keeps_press_in_offspace )
        {
            // reserve key
            root->reserve_key( key, true );
            this->reserve_hover_keys( root, true );
        }
        
        this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input / Press Activation -> input type changed to offspace." );
        
        this->ie_start_activation();
    }
    else if( this->_active == key && press && ( !hit || offspace ) )
    { // moved to offspace
        if( this->keeps_press_in_offspace )
        {
            offspace = true;
        }
        else
        {
            // release it
            if( this->_active )
            {
                this->input_treeRefresh();
                this->cm.log( SRC_INFO, Defs::Log::Input, "Queue refresh (activation moved to offspace)." );
            }
            this->_active = std::nullopt;
            //this->reserve_hover_keys( root, false );
            this->ie_stop_activation( false );
        }
    }
    else if( this->_active == key && !press )
    { // released
        this->input_treeRefresh();
        this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input / Release Activation (real: ", real ,") -> queue refresh." );
        
        this->_active = std::nullopt;
        if( this->keeps_press_in_offspace )
            this->reserve_hover_keys( root, false );
        this->ie_stop_activation( real );
    }
    
    if( !this->_fallthrough )
    {
        if( hit )
        {
            offspace = true;
            this->cm.log( SRC_INFO, Defs::Log::Input, "Block input (change it to offspace because fallthrough is not enabled)." );
        }
    }
}

void Position_InputNode::interruptActivation()
{
    this->cm.log( SRC_INFO, Defs::Log::Input, "Activation interrupted from outside code." );
    this->_active = std::nullopt;
    this->ie_stop_activation( false );
    this->input_treeRefresh();
}

}
