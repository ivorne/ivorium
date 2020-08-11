#include "InputSystem.hpp"

#include <ivorium_core/ivorium_core.hpp>

#include "../InputNode.hpp"
#include "../SourceInputNode.hpp"
#include "../../Defs.hpp"

namespace iv
{

InputSystem::InputSystem( SystemContainer * sc, InputSource * source ) :
    System( sc ),
    source( source ),
    _window_size( 1, 1 ),
    last_check_frame_id( 0 )
{
}

void InputSystem::status( TextDebugView * view )
{
}

bool InputSystem::flushSystem()
{
    if( this->log_enabled( iv::Defs::Log::ConsistencyChecks_Frame ) )
    {
        unsigned frame_id = this->system_container()->frame_id();
        if( this->last_check_frame_id != frame_id )
        {
            this->last_check_frame_id = frame_id;
            
            std::unordered_set< InputNode * > roots;
            for( InputNode * node : this->nodes )
                if( !node->input_getParent() )
                    roots.insert( node );
            
            for( InputNode * root : roots )
            {
                if( !root->quiet() && !dynamic_cast< SourceInputNode * >( root ) )
                {
                    this->log( SRC_INFO, iv::Defs::Log::ConsistencyChecks_Frame, "InputNode ", root->cm, " is disconnected." );
                }
            }
        }
    }
    
    return false;
}

/*
void InputSystem::print_node( TreeDebugView * tree, InputNode * node )
{
    for( InputNode * child : node->_children )
    {
        tree->Push( child->instance()->instance_name().c_str() );
        child->cm.inheritance_root()->print_status_with_inherited( (TextDebugView*)tree, &child->cm );
        this->print_node( tree, child );
        tree->Pop();
    }
}

void InputSystem::status( TextDebugView * view )
{
    TreeDebugView tree( view->context() );
    
    for( InputClient * root : this->input_listeners )
    {
        auto source = dynamic_cast< SourceInputNode * >( root );
        if( !source )
        {
            tree.Push( root->instance()->instance_name().c_str() );
            tree.out() << "<unknown>" << std::endl;
            tree.Pop();
        }
        else
        {
            tree.Push( source->instance()->instance_name().c_str() );
            source->cm.print_status_with_inherited( (TextDebugView*)&tree, &source->cm );
            this->print_node( &tree, source );
            tree.Pop();
        }
    }
    
    tree.Write( TreeDebugView::Style::BoldFramesWeakLinks, view );
}
*/

int2 InputSystem::window_size( )
{
    return this->_window_size;
}

void InputSystem::window_size( int2 val )
{
    this->_window_size = val;
}

int2 InputSystem::input_position( Input::Key key, int device_id )
{
    return this->source->input_position( key, device_id );
}

float InputSystem::input_value( Input::Key key, int device_id )
{
    return this->source->input_value( key, device_id );
}

unsigned InputSystem::input_character()
{
    return this->source->input_character();
}

void InputSystem::node_register( InputNode * node )
{
    this->nodes.insert( node );
}

void InputSystem::node_unregister( InputNode * node )
{
    this->nodes.erase( node );
}

}
