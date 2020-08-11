#include "ElementSystem.hpp"
#include "../Defs.hpp"

namespace iv
{

ElementSystem::ElementSystem( SystemContainer * sc ) :
    System( sc )
{
}

bool ElementSystem::flushSystem()
{
    if( this->log_enabled( iv::Defs::Log::ConsistencyChecks_Frame ) )
    {
        unsigned frame_id = this->system_container()->frame_id();
        if( this->last_check_frame_id != frame_id )
        {
            this->last_check_frame_id = frame_id;
            
            std::unordered_set< Elem * > roots = this->elems;
            
            for( Elem * elem : this->elems )
            {
                elem->elem_eachChild(
                    [&]( Elem * child )
                    {
                        roots.erase( child );
                    }
                );
            }
            
            for( Elem * root : roots )
            {
                if( !root->quiet() && root->first_pass_frame_id() != frame_id )
                {
                    this->log( SRC_INFO, iv::Defs::Log::ConsistencyChecks_Frame, "Element ", root->cm, " is disconnected." );
                }
            }
        }
    }
    
    return false;
}

void ElementSystem::elem_register( Elem * elem )
{
    this->elems.insert( elem );
}

void ElementSystem::elem_unregister( Elem * elem )
{
    this->elems.erase( elem );
}

void ElementSystem::print_elem_children( TreeDebugView * tree, Elem * elem, std::unordered_set< Elem * > & printed )
{
    elem->elem_eachChild(
        [&]( Elem * child )
        {
            if( !printed.count( child ) )
            {
                printed.insert( child );
                
                tree->Push( child->cm.instance()->instance_name().c_str() );
                child->cm.inheritance_root()->print_status_with_inherited( (TextDebugView*)tree, &child->cm );
                this->print_elem_children( tree, child, printed );
                tree->Pop();
            }
        }
    );
}

void ElementSystem::status( TextDebugView * view )
{
    std::unordered_set< Elem * > printed;
    
    TreeDebugView tree( view->context() );
    
    for( Camera * camera : this->cameras )
    {
        tree.Push( camera->cm.instance()->instance_name().c_str() );
        camera->cm.print_status_with_inherited( (TextDebugView*)&tree, &camera->cm );
        this->print_elem_children( &tree, camera, printed );
        tree.Pop();
    }
    
    tree.Write( TreeDebugView::Style::BoldFramesWeakLinks, view );
}

void ElementSystem::camera_created( Camera * camera )
{
    this->cameras.insert( camera );
}

void ElementSystem::camera_destroyed( Camera * camera )
{
    this->cameras.erase( camera );
}

}
