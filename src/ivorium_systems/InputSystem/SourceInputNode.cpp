#include "SourceInputNode.hpp"
#include "../Defs.hpp"

namespace iv
{

SourceInputNode::SourceInputNode( Instance * inst ) :
    InputNode( inst ),
    InputBindingListener( inst ),
    FrameUpdateClient( inst ),
    cm( inst, this, "InputSourceNode" ),
    pressed(),
    reserved(),
    refresh_queued( false )
{
    this->cm.inherits( this->InputNode::cm, this->FrameUpdateClient::cm );
    
    this->pressed.insert( std::pair( Input::Key::Mouse, 0 ) );
}

SourceInputNode::~SourceInputNode()
{
    this->input_deactivate();
}

void SourceInputNode::Add_InputNode( InputNode * node )
{
    // disconnect node from previous parent
    if( node->input_getParent() )
        node->input_getParent()->input_childDisconnect( node );
    
    //
    this->_input_children.push_back( node );
    node->input_setParent( this );
}

void SourceInputNode::Remove_InputNode( InputNode * node )
{
    this->input_childDisconnect( node );
}

void SourceInputNode::input_childDisconnect( InputNode * node )
{
    for( size_t i = 0; i < this->_input_children.size(); i++ )
        if( this->_input_children[ i ] == node )
        {
            this->_input_children[ i ]->input_setParent( nullptr );
            this->_input_children.erase( this->_input_children.begin() + i );
            break;
        }
}

void SourceInputNode::input_eachChild( std::function< void( InputNode * ) > const & f )
{
    for( auto it = this->_input_children.rbegin(); it != this->_input_children.rend(); ++it )
        f( *it );
}

void SourceInputNode::reserve_key( Input::DeviceKey key, bool reserve )
{
    if( reserve )
    {
        if( key.second >= 0 )
            this->reserved.insert( key );
        else
            this->reserved_whole.insert( key.first );
    }
    else
    {
        if( key.second >= 0 )
            this->reserved.erase( key );
        else
            this->reserved_whole.erase( key.first );
    }
}

void SourceInputNode::on_binding_changed()
{
    this->refresh_queued = true;
}

void SourceInputNode::treeRefresh()
{
    this->refresh_queued = true;
}

void SourceInputNode::frame_update()
{
    if( this->refresh_queued )
    {
        this->refresh_queued = false;
        this->do_refresh();
    }
}

bool SourceInputNode::input( Input const * input )
{
    Input::DeviceKey device_key( input->key(), input->device_id() );
    this->reserved.erase( device_key );
    
    // 
    this->cm.log( SRC_INFO, Defs::Log::Input, input->type(), " ", input->key(), "/", input->device_id() );
    
    if( input->type() == Input::Type::Trigger )
    {
        return !this->input_trigger_visit( this, device_key );
    }
    else
    {
        // update list of active keys
        if( input->type() == Input::Type::Press )
        {
            if( this->pressed.count( device_key ) )
                return false;
            
            this->pressed.insert( device_key );
        }
        else if( input->type() == Input::Type::Release )
        {
            if( !this->pressed.count( device_key ) )
                return false;
            
            this->pressed.erase( device_key );
        }
        
        // visit
        bool press = input->type() == Input::Type::Press;
        bool real = true;
        bool offspace = false;
        
        bool cont = this->input_visit( this, device_key, press, real, offspace );
        
        return !cont || !real;
    }
}

bool SourceInputNode::is_reserved( Input::DeviceKey device_key )
{
    return this->reserved.count( device_key ) || this->reserved_whole.count( device_key.first );
}

void SourceInputNode::do_refresh()
{
    for( Input::DeviceKey const & device_key : this->pressed )
    {
        bool press = true;
        bool real = false;
        bool offspace = this->is_reserved( device_key );

        this->input_visit( this, device_key, press, real, offspace );
    }
}

void SourceInputNode::deactivateSubtree( InputNode * node )
{
    for( Input::DeviceKey const & device_key : this->pressed )
    {
        bool press = false;
        bool real = false;
        bool offspace = this->is_reserved( device_key );
        
        node->input_visit( nullptr, device_key, press, real, offspace );
    }
}

InputRoot * SourceInputNode::input_getRoot()
{
    return this;
}

}
