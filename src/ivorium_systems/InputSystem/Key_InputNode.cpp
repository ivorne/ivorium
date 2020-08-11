#include "Key_InputNode.hpp"
#include "../Defs.hpp"

namespace iv
{

Key_InputNode::Key_InputNode( Instance * inst, InputId input ) :
    InputNode( inst ),
    InputEvent( inst ),
    cm( inst, this, "Key_InputNode" ),
    ibq( inst ),
    _input_id(),
    _fallthrough( false ),
    _active( std::nullopt )
{
    this->cm.inherits( this->InputNode::cm, this->InputEvent::cm );
    this->cm.owns( this->ibq.cm );
    
    this->input_id( input );
}

Key_InputNode::~Key_InputNode()
{
}

void Key_InputNode::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "Key_InputNode" );
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "input_id", this->input_id() );
    row.Column( "fallthrough_enabled", this->fallthrough_enabled() );
    row.Column( "active", this->_active );
}

void Key_InputNode::input_id( InputId val )
{
    this->_input_id = val;
    this->input_treeRefresh();
}

InputId Key_InputNode::input_id()
{
    return this->_input_id;
}

void Key_InputNode::fallthrough_enabled( bool val )
{
    this->_fallthrough = val;
    this->input_treeRefresh();
}

bool Key_InputNode::fallthrough_enabled()
{
    return this->_fallthrough;
}

bool Key_InputNode::input_trigger_process( InputRoot * root, Input::DeviceKey key )
{
    if( !this->ibq.IsBound( this->_input_id, key ) )
        return true;
    
    this->ie_trigger();
    
    return false;
}

void Key_InputNode::input_process( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace )
{
    if( !this->ibq.IsBound( this->_input_id, key ) )
    {
        this->cm.log( SRC_INFO, Defs::Log::Input, "Refuse input (not bound)." );
        return;
    }
    
    if( press )
    {
        if( !this->_active.has_value() || this->_active.value() == key )
        { // first key
            this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input (change input to fake release)." );
            
            this->_active = key;
            this->ie_start_duration();
            if( real )
                this->ie_start_activation();
            
            //
            press = false;
            real = false;
        }
        else
        { // other keys
            if( !this->_fallthrough )
            {
                press = false;
                real = false;
                
                this->cm.log( SRC_INFO, Defs::Log::Input, "Block input (change it to fake release because fallthrough is not enabled)." );
            }
            else
            {
                this->cm.log( SRC_INFO, Defs::Log::Input, "Fallthrough input." );
            }
        }
    }
    else
    {
        if( this->_active == key )
        { // first key
            this->cm.log( SRC_INFO, Defs::Log::Input, "Accept input (change release to fake)." );
            
            this->_active = std::nullopt;
            this->ie_stop_duration();
            this->ie_stop_activation( real );
            
            if( this->_fallthrough )
                this->input_treeRefresh();
            
            real = false;
        }
        else
        { // other keys
            if( !this->_fallthrough )
            {
                this->cm.log( SRC_INFO, Defs::Log::Input, "Block input (change release to fake because fallthrough is not enabled)." );
                real = false;
            }
            else
            {
                this->cm.log( SRC_INFO, Defs::Log::Input, "Fallthrough input." );
            }
        }
    }
}

void Key_InputNode::interruptActivation()
{
    this->cm.log( SRC_INFO, Defs::Log::Input, "Activation interrupted from outside code." );
    this->_active = std::nullopt;
    this->ie_stop_activation( false );
    this->input_treeRefresh();
}

}
