#include "InputNode.hpp"
#include "../Defs.hpp"

namespace iv
{

InputNode::InputNode( Instance * inst ) :
    cm( inst, this, "InputNode", ClientMarker::Status() ),
    inst( inst ),
    _enabled( true ),
    _parent( nullptr ),
    _quiet( false )
{
}

InputNode::~InputNode()
{
    if( this->_parent )
        this->_parent->input_childDisconnect( this );
}

Instance * InputNode::instance() const
{
    return this->inst;
}

void InputNode::status( iv::TableDebugView * view )
{
    static iv::TableId DebugTable = TableId::create( "InputNode" );
    auto row = view->Table( DebugTable ).Row( this );
    
    row.Column( "inputEnabled", this->inputEnabled() );
}

InputNode * InputNode::input_getParent()
{
    return this->_parent;
}

void InputNode::input_setParent( InputNode * parent )
{
    this->_parent = parent;
}

InputRoot * InputNode::input_getRoot()
{
    if( this->_parent )
        return this->_parent->input_getRoot();
    else
        return nullptr;
}

void InputNode::inputEnabled( bool value )
{
    if( this->_enabled == value )
        return;
    
    this->_enabled = value;
    this->input_treeRefresh();
}

bool InputNode::inputEnabled()
{
    return this->_enabled;
}

void InputNode::input_treeRefresh()
{
    if( auto root = this->input_getRoot() )
        root->treeRefresh();
}

void InputNode::input_deactivate()
{
    if( auto root = this->input_getRoot() )
        root->deactivateSubtree( this );
}

void InputNode::quiet( bool val )
{
    this->_quiet = val;
}

bool InputNode::quiet() const
{
    return this->_quiet;
}

//---------------
bool InputNode::input_visit( InputRoot * root, Input::DeviceKey key, bool & press, bool & real, bool & offspace )
{
    if( !this->_enabled )
        return true;
    
    // try to process it
    this->cm.log( SRC_INFO, Defs::Log::InputTree, press ? "press" : "release", " ", real ? " " : "(fake) ", offspace ? "(offspace) " : " ", key.first, "/", key.second, "" );
    
    this->input_process( root, key, press, real, offspace );
    
    this->input_eachChild(
        [&]( InputNode * node )
        {
            node->input_visit( root, key, press, real, offspace );
        }
    );
    
    return true;
}

bool InputNode::input_trigger_visit( InputRoot * root, Input::DeviceKey key )
{
    if( !this->_enabled )
        return true;
    
    bool cont = this->input_trigger_process( root, key );
    
    this->cm.log( SRC_INFO, Defs::Log::InputTree, "trigger ", key.first, "/", key.second, " -> ", cont ? "continue" : "stop" );
    
    if( !cont )
        return false;
    
    bool keep_going = true;
    this->input_eachChild(
        [&]( InputNode * node )
        {
            keep_going = keep_going && node->input_trigger_visit( root, key );
        }
    );
    if( !keep_going )
        return false;
    
    return true;
}

}
